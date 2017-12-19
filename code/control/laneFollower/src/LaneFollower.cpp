/**
 * LaneFollower - LaneFollower code.
 * Copyright (C) 2016 Christian Berger
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "LaneFollower.h"

namespace carolocup
{
	namespace control
	{
		using namespace std;
		using namespace cv;
		using namespace odcore::base;
		using namespace odcore::data;
		using namespace odcore::data::image;
		using namespace odcore::wrapper;
		using namespace odcore::data::dmcp;
		using namespace gap;

		LaneFollower::LaneFollower(const int &argc, char **argv)
				: TimeTriggeredConferenceClientModule(argc, argv, "carolocup-lanefollower"),
				  m_debug(false),
				  Sim(false),
				  pid_tuning(0),
				  m_previousTime(),
				  m_eSum(0),
				  m_eOld(0),

				  m_vehicleControl(),
				  laneFollowerMSG(),

				  p_gain(0),       // The gain values for the PID control algorithm
				  i_gain(0),
				  d_gain(0),

				  _state(0),
				  _isOvertaking(0),

				  stop(),
				  stopCounter(0),
				  state(MOVING),
				  prevState(MOVING),
				  inRightLane(true),
				  currentDistance(),
				  _stop(0),
				  speed(0),
				  steer(0),
				  imgProcess(),
				  auto_pid(),
				  param(),
				  dp(),
				  best_err(0),
				  err(0),
				  sum(0),
				  firstIteration(0),
				  iteration(0),
				  t_state(BEGIN)
		{}

		LaneFollower::~LaneFollower()
		{}

		void LaneFollower::setUp()
		{
			// Get configuration data for this class.
			KeyValueConfiguration kv = getKeyValueConfiguration();
			m_debug = kv.getValue<int32_t>("global.debug") == 1;
			Sim = kv.getValue<int32_t>("global.sim") == 1;
			pid_tuning = kv.getValue<int32_t>("global.laneFollower.pid_tuning");
			string TYPE = kv.getValue<string>("global.cameraproxy.camera.type");
			int cam_type = 0;
			if (TYPE.compare("opencv") == 0)
			{
				cam_type = 1;
			}
			else if (TYPE.compare("ueye") == 0)
			{
				cam_type = 0;
			}

			if (pid_tuning < 2)
			{
				p_gain = kv.getValue<double>("global.laneFollower.p_gain");

				i_gain = kv.getValue<double>("global.laneFollower.i_gain");

				d_gain = kv.getValue<double>("global.laneFollower.d_gain");

				param[0] = p_gain;
				param[1] = i_gain;
				param[2] = d_gain;

				dp[0] = 1.0;
				dp[1] = 1.0;
				dp[2] = 1.0;
			}
			else
			{
				param[0] = 0;
				param[1] = 0;
				param[2] = 0;

				dp[0] = 1.0;
				dp[1] = 1.0;
				dp[2] = 1.0;
			}



			// TODO auto_pid.init(p_gain, i_gain, d_gain);

			// Setup window for debugging if debug flag set
			if (m_debug)
			{
				imgProcess.setUpWindowDebug();
			}
			// Values adjusted for simulation environment, if sim flag is set
			if (Sim)
			{
				_state = 1;
			}

			imgProcess.m_debug = &m_debug;
			imgProcess.Sim = &Sim;

			imgProcess.p_gain = &p_gain;
			imgProcess.i_gain = &i_gain;
			imgProcess.d_gain = &d_gain;

			imgProcess._isOvertaker = &_isOvertaking;
			imgProcess.stopCounter = &stopCounter;

			imgProcess._speed = &speed;
			imgProcess._steer = &steer;

			imgProcess.camType = &cam_type;

			imgProcess.inRightLane = &inRightLane;

			stop = &imgProcess.stop;
			currentDistance = &imgProcess.currentDistance;

		}

		void LaneFollower::tearDown()
		{
			if (m_debug)
			{
				imgProcess.destroyWindowDebug();
			}
		}

		odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode LaneFollower::body()
		{
			while (getModuleStateAndWaitForRemainingTimeInTimeslice() == ModuleStateMessage::RUNNING)
			{
				TimeStamp now;

				Container communicationLinkContainer = getKeyValueDataStore().get(CommunicationLinkMSG::ID());
				if (communicationLinkContainer.getDataType() == CommunicationLinkMSG::ID())
				{
					const CommunicationLinkMSG communicationLinkMSG = communicationLinkContainer.getData<CommunicationLinkMSG>();
					_state = communicationLinkMSG.getStateLaneFollower();
					_isOvertaking = communicationLinkMSG.getStateOvertaker();

					if (pid_tuning == 2)
					{
						p_gain = communicationLinkMSG.getP();
						cerr << now.getYYYYMMDD_HHMMSS_noBlank() << " new p_gain " << p_gain << endl;

						i_gain = communicationLinkMSG.getI();
						cerr << now.getYYYYMMDD_HHMMSS_noBlank() << " new i_gain " << i_gain << endl;

						d_gain = communicationLinkMSG.getD();
						cerr << now.getYYYYMMDD_HHMMSS_noBlank() << " new d_gain " << d_gain << endl;

						param[0] = p_gain;
						param[1] = p_gain;
						param[2] = p_gain;

						ofstream pid_file;
						pid_file.open("/opt/configuration/pid.txt");
						pid_file << "lanefollowerp=" + to_string(p_gain) + "\n" +
									"lanefolloweri=" + to_string(i_gain) + "\n" +
									"lanefollowerd=" + to_string(d_gain) + "\n";
						pid_file.close();
					}

				}

				if (_state == 0)//TODO flip to one
				{
					//cerr << now.getYYYYMMDD_HHMMSS_noBlank() << " Lanefollower _state " << _state << " && m_debug "
						 //<< m_debug << endl;
					bool has_next_frame = false;
					_stop = 0;

					// Get the most recent available container for a SharedImage.
					Container image_container = getKeyValueDataStore().get(SharedImage::ID());

					if (image_container.getDataType() == SharedImage::ID())
					{
						has_next_frame = imgProcess.readSharedImage(image_container);
					}
					// If we have an image from the previous call, it is then processed, error state calculated and desired steering derived from the error
					if (has_next_frame)
					{
						imgProcess.processImage();
						double error = imgProcess.errorCalculation();

						if (!firstIteration)
						{
							best_err = error;
							firstIteration = 1;
						}

						if (t_state == BEGIN)
						{
							sum = (dp[0]+dp[1]+dp[2]);
							t_state = UPDATE_ERROR_1;
							cerr << now.getYYYYMMDD_HHMMSS_noBlank() << " TWIDDLE SUM " <<  t_state << endl;
						}

						if (pid_tuning == 0)
						{
							if (sum > 0.001) //0.000000001
							{
								cerr << now.getYYYYMMDD_HHMMSS_noBlank() << " TWIDDLE CALIBRATING " <<  t_state << endl;
								switch (t_state)
								{
									case UPDATE_ERROR_1:
										param[iteration] += dp[iteration];
										t_state = UPDATE_ERROR_2;
										break;
									case UPDATE_ERROR_2:
										err = error;
										if (err<best_err)
										{
											best_err = err;
											dp[iteration] *= 1.1;

											//NEXT
											t_state = LOOP;
										}
										else
										{
											param[iteration] -= 2*dp[iteration];
											if (param[iteration] < 0.0)
											{
												param[iteration] = 0.0;
											}
											t_state = UPDATE_ERROR_3;
										}
										break;
									case UPDATE_ERROR_3:
										err = error;

										if (err<best_err)
										{
											best_err = err;
											dp[iteration] *= 1.05;
										}
										else
										{
											param[iteration] += dp[iteration];
											dp[iteration] *= 0.95;
										}

										//NEXT
										t_state = LOOP;

										break;
									case LOOP:
										iteration++;
										t_state = UPDATE_ERROR_1;
										if (iteration > 2) {
											iteration = 0;
											t_state = BEGIN;
										}
										cerr << now.getYYYYMMDD_HHMMSS_noBlank() << " TWIDDLE ITERATION " <<  iteration << endl;
										break;
									case BEGIN:

										break;
									case COMPLETE:

										break;
									default:
										break;
								}
							}
							else
							{
								t_state = COMPLETE;
								cerr << now.getYYYYMMDD_HHMMSS_noBlank() << " TWIDDLE DONE " << endl;
							}

							//cerr << now.getYYYYMMDD_HHMMSS_noBlank() << " SPEED " << auto_pid.outputThrottle(31.0) << endl;

							p_gain = param[0];
							i_gain = param[1];
							d_gain = param[2];

							cerr << now.getYYYYMMDD_HHMMSS_noBlank() << " KP " << p_gain << endl;
							cerr << now.getYYYYMMDD_HHMMSS_noBlank() << " KI " << i_gain << endl;
							cerr << now.getYYYYMMDD_HHMMSS_noBlank() << " KD " << d_gain << endl;
						}
						laneFollower(error);

					}

					state_machine();

					cerr << now.getYYYYMMDD_HHMMSS_noBlank() << " STEER " << 90 + (steer * (180 / 3.14)) << endl;

					laneFollowerMSG.setDistanceToRightLane(*currentDistance);
					laneFollowerMSG.setStateLane(inRightLane);

					sendToConference(true);
				}
				else if (!_state && !_stop)
				{
					_stop = 1;
					m_vehicleControl.setBrakeLights(true);
					sendToConference(false);
				}
			}
			return ModuleExitCodeMessage::OKAY;
		}

		//##############################################
		// PID control algorithm
		void LaneFollower::laneFollower(double e)
		{
			TimeStamp currentTime;
			double timeStep = (currentTime.toMicroseconds() - m_previousTime.toMicroseconds()) / (1000.0 * 1000.0);
			m_previousTime = currentTime;

			// A way to handle toggling in corners
			if (fabs(e) < 1e-2)
			{
				m_eSum = 0;
			}
			else
			{
				m_eSum += e;
			}

			// PID control algorithm uses the following values, with the meaning:
			//Kp = p_gain -> Proportional -> the oscillation in the trajectory as the algorithm attempts to approach the error state to 0, moving towards the goal
			//Ki = i_gain-> Integral -> Current -> how long/ how much of the oscillation is spent on either side of the goal, "look at the past"
			//Kd = d_gain-> derivative -> how sharp the approximation to the goal is made, directly influencing the oscillation
			const double p = p_gain * e;
			const double i = i_gain * timeStep * m_eSum;
			const double d = d_gain * (e - m_eOld) / timeStep;

			cerr << currentTime.getYYYYMMDD_HHMMSS_noBlank() << " ERROR -> " << e << endl;
			m_eOld = e;

			const double y = p + i + d;

			double desiredSteering = 0;

			if (fabs(e) > 1e-2)
			{
				desiredSteering = y;
			}
			// Set an upper and lower limit for the desired steering
			if (desiredSteering > 1.5)
			{
				desiredSteering = 1.5;
			}
			if (desiredSteering < -1.5)
			{
				desiredSteering = -1.5;
			}

			imgProcess.showDebugImage();

			m_vehicleControl.setSteeringWheelAngle(desiredSteering);
			steer = desiredSteering;
		}

		void LaneFollower::state_machine()
		{
			state = static_cast<StateMachine>(imgProcess.getState());
			// State control algorithm, attempts to stop briefly at each intersection and stops the car if danger zone (image processing could not find a lane to follow)
			if (state == MOVING)
			{
				//TODO laneFollowerMSG.setDanger(0);
				if (Sim)
				{
					if (*stop)
					{
						state = STOP;
						imgProcess.setState(STOP);
						stopCounter = 0;
					}
					else
					{
						m_vehicleControl.setBrakeLights(false);
						m_vehicleControl.setSpeed(1);
						speed = 1;
						prevState = MOVING;
						imgProcess.setPrev_State(MOVING);
						state = MOVING;
						imgProcess.setState(MOVING);
					}
				}
				else
				{
					if (*stop)
					{
						state = STOP;
						imgProcess.setState(STOP);
						stopCounter = 0;
					}
					else
					{
						m_vehicleControl.setBrakeLights(false);
						m_vehicleControl.setSpeed(_SPEED);
						speed = _SPEED;
						prevState = MOVING;
						imgProcess.setPrev_State(MOVING);
						state = MOVING;
						imgProcess.setState(MOVING);
					}
				}
			}

			if (state == STOP)
			{
				m_vehicleControl.setSteeringWheelAngle(0);
				steer = 0;
				if (Sim)
				{
					if (stopCounter < 5.9999)
					{
						stopCounter += 0.5;
					}
					else
					{
						stopCounter += 0.5;
						m_vehicleControl.setSpeed(0);
						speed = 0;
					}
					if (stopCounter > 40.9999)
					{
						state = RESUME;
						prevState = STOP_LINE;
						imgProcess.setPrev_State(STOP_LINE);
						imgProcess.setState(RESUME);
					}
				}
				else
				{
//TODO laneFollowerMSG.setDanger(1);
					m_vehicleControl.setBrakeLights(true);
					stopCounter += 0.5;

					if (stopCounter > 20.9999)
					{
						state = RESUME;
						prevState = STOP_LINE;
						imgProcess.setPrev_State(STOP_LINE);
						imgProcess.setState(RESUME);
					}
				}
			}
			if (state == RESUME)
			{
//TODO laneFollowerMSG.setDanger(0);
				m_vehicleControl.setBrakeLights(false);
				if (Sim)
				{
					if (stopCounter < 65.9999)
					{
						stopCounter += 0.5;
						m_vehicleControl.setSpeed(1);
						speed = 1;
						m_vehicleControl.setSteeringWheelAngle(0);
						steer = 0;
					}
					else
					{
						state = MOVING;
						imgProcess.setState(MOVING);
						m_vehicleControl.setBrakeLights(false);
						m_vehicleControl.setSpeed(1);
						speed = 1;
					}
				}
				else
				{
					if (stopCounter < 45.9999)
					{
						stopCounter += 0.5;
						m_vehicleControl.setSpeed(_SPEED);
						speed = _SPEED;
						m_vehicleControl.setSteeringWheelAngle(0.24);
						steer = 0.24;

					}
					else
					{
						state = MOVING;
						imgProcess.setState(MOVING);
// m_vehicleControl.setBrakeLights(false);
						m_vehicleControl.setSpeed(_SPEED);
						speed = _SPEED;
					}
				}
			}
			if (state == DANGER)
			{
// After a stop line, go forward and don't steer at all, it is expected to not find any reference lane markings for a stretch of road following an intersection
				if (prevState == STOP_LINE)
				{
//TODO laneFollowerMSG.setDanger(0);
					m_vehicleControl.setBrakeLights(false);
					m_vehicleControl.setSpeed(_SPEED);
					speed = _SPEED;
					m_vehicleControl.setSteeringWheelAngle(0);
					steer = 0;
				}
// If the previous state is moving, lane markings were not found in image processing and car applies brake
				if (prevState == MOVING)
				{
					prevState = DANGER;
					imgProcess.setPrev_State(DANGER);
					m_vehicleControl.setBrakeLights(true);
//TODO laneFollowerMSG.setDanger(1);
				}
			}
		}

		void LaneFollower::sendToConference(bool sendMSG)
		{
			if (sendMSG)
			{
				Container lfMessage(laneFollowerMSG);
				getConference().send(lfMessage);
			}
			// Create container for finally sending the set values for the control algorithm.
			Container c(m_vehicleControl);
			// Send container
			getConference().send(c);
		}
	}
} // carolocup::control
