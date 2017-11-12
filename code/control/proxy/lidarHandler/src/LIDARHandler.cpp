/**
 * LIDARHandler - LIDARHandler code.
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

#include <stdint.h>

#include <iostream>

#include "odvdcarolocupdatamodel/generated/chalmersrevere/carolocup/ExampleMessage.h"

#include "LIDARHandler.h"

namespace carolocup
{
	namespace control
	{

		using namespace std;
		using namespace odcore;
		using namespace odcore::base;
		using namespace odcore::base::module;
		using namespace odcore::data;
		using namespace odcore::wrapper;
		using namespace sweep;
		using namespace gap;

		LIDARHandler::LIDARHandler(const int &argc, char **argv)
				: TimeTriggeredConferenceClientModule(argc, argv, "carolocup-lidarhandler"),
				  device{PORT},
				  lidar(),
				  strength(),
				  lidar_data(),
				  count(0),
				  m_start_azimuth(0.0),
				  m_end_azimuth(0.0),
				  d(""),
				  cpc(),
				  lidarMSG()
		{}

		LIDARHandler::~LIDARHandler()
		{}

		void LIDARHandler::setUp()
		{
			startDevice();
		}

		void LIDARHandler::tearDown()
		{
			stopDevice();
		}

		odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode LIDARHandler::body()
		{

			while (getModuleStateAndWaitForRemainingTimeInTimeslice() ==
				   odcore::data::dmcp::ModuleStateMessage::RUNNING)
			{
				//cout << "Inside the main processing loop!." << endl;
				try
				{
					if (count == 3)
					{
						for (int i = 0; i <= 130; ++i)
						{
							if (lidar[i].size() > 0)
							{
								sort(lidar[i].begin(), lidar[i].end());
								int med = (int) lidar[i].size() / 2;
								lidar_data[i] = (double) lidar[i][med];
							}
							lidar[i].clear();
						}

						for (int i = 230; i <= 360; ++i)
						{
							if (lidar[i].size() > 0)
							{
								sort(lidar[i].begin(), lidar[i].end());
								int med = (int) lidar[i].size() / 2;
								lidar_data[i] = (double) lidar[i][med];
							}
							lidar[i].clear();
						}

						sendLIDARData(lidar_data);
					}

					const sweep::scan scan = device.get_scan();

					for (const sweep::sample &sample : scan.samples)
					{
//                        std::cout << "angle "
//                                  << (int) std::fmod((static_cast<double>(sample.angle) / 1000. + 90.), 360.)
//                                  << " distance " << sample.distance << " strength "
//                                  << sample.signal_strength << "\n";

						int angle = (sample.angle / 1000) % 360;
						int str = sample.signal_strength;
						if (angle < 131 || angle > 229)
						{
							int distance = sample.distance;
							if (distance > MIN_DIS && distance < MAX_DIS)
							{
								lidar[angle].push_back(distance);
								strength[angle].push_back(str);
							}
							else
							{
								lidar[angle].push_back(-1);
								strength[angle].push_back(str);
							}

//							std::cout << "angle-"
//									  << angle
//									  << " distance ->" << distance << " strength "
//									  << sample.signal_strength << endl;
						}
						TimeStamp now;

						cpc.setStartAzimuth(m_start_azimuth);
						m_end_azimuth = 360;
						cpc.setEndAzimuth(m_next_azimuth);
						cpc.setEntriesPerAzimuth(1);
						cpc.setDistances(d);

						Container container(cpc);
						container.setSampleTimeStamp(now);
						getConference().send(container);
						count++;
					}
				}
				catch (const sweep::device_error &e)
				{
					std::cerr << "Error: " << e.what() << std::endl;
					exit(EXIT_FAILURE);
				}
			}

			return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
		}

		void LIDARHandler::startDevice()
		{
			try
			{
				cout << "Constructing sweep device..." << endl;
				device.set_motor_speed(1);
				device.set_sample_rate(500);
				device.set_motor_speed(10);
				cout << "Motor Speed Setting: " << device.get_motor_speed() << " Hz" << endl;
				cout << "Sample Rate Setting: " << device.get_sample_rate() << " Hz" << endl;

				cout << "Beginning data acquisition as soon as motor speed stabilizes..." << endl;
				device.start_scanning();
			}
			catch (const sweep::device_error &e)
			{
				cerr << "Error: " << e.what() << endl;
				exit(EXIT_FAILURE);
			}
		}

		void LIDARHandler::stopDevice()
		{
			try
			{
				device.stop_scanning();
			}
			catch (const sweep::device_error &e)
			{
				cerr << "Error: " << e.what() << endl;
			}
		}

		void LIDARHandler::sendLIDARData(map<uint32_t, double> LIDAR_data)
		{
			lidarMSG.setMapOfLidarDistances(LIDAR_data);
			Container c(lidarMSG);
			getConference().send(c);

			count = 0;
		}
	}
} // carolocup::control
