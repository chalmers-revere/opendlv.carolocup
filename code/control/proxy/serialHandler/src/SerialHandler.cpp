/**
 * SerialHandler - SerialHandler code.
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

#include "SerialHandler.h"

namespace carolocup
{
	namespace control
	{

		using namespace std;
		using namespace odcore::base;
		using namespace odcore::data;
		using namespace odcore::data::image;
		using namespace odcore::wrapper;
		using namespace odcore::data::dmcp;
		using namespace gap;
		using namespace automotive::miniature;

		SerialHandler::SerialHandler(const int &argc, char **argv)
				: DataTriggeredConferenceClientModule(argc, argv, "carolocup-serialhandler"),
				  serial(),
				  motor(0),
				  servo(90),
				  lights(-1),
				  brake(true),
				  sbd(),
				  sensors(),
				  raw_sensors(),
				  odometerCounter(0),
				  km(0),
				  isSensorValues(false),
				  _debug(false),
				  serialBehaviour("dummy")
		{
			if (argc >= 4)
			{
				SERIAL_PORT = argv[3];
			}
			else
			{
				cerr << "Usage: " << argv[0] << "SOURCE DESTINATION (DEV PORT)" << endl;
			}
		}

		SerialHandler::~SerialHandler()
		{}

		void SerialHandler::setUp()
		{
			try
			{
				KeyValueConfiguration kv = getKeyValueConfiguration();

				_debug = kv.getValue<int32_t>("global.debug") == 1;

				if (_debug)
				{
#define DEBUG
				}

				const string _S_PORT = SERIAL_PORT;

				cerr << "Setting up serial handler to port " << SERIAL_PORT << endl;

				this->serial = serial_new();

				this->serial->on_write = &__on_write;
				this->serial->on_read = &__on_read;

				const char *_port = _S_PORT.c_str();
				serial_open(this->serial, _port, BAUD_RATE);

				uint8_t rb = serial_handshake(this->serial, 'a', 's'); //a = actuators, s = sensors

				if (rb == 'a')
				{
					serialBehaviour = "out";
				}
				else if (rb == 's')
				{
					serialBehaviour = "in";
				}

				odcore::base::Thread::usleepFor(5 * ONE_SECOND);


				protocol_data d_motor;
				d_motor.id = ID_OUT_MOTOR;
				d_motor.value = MOTOR_IDLE;
				d_motor.sub_id = NO_DATA;

				serial_send(this->serial, d_motor);

				protocol_data d_servo;
				d_servo.id = ID_OUT_SERVO;
				d_servo.value = STRAIGHT_DEGREES;
				d_servo.sub_id = NO_DATA;

				serial_send(this->serial, d_servo);


				odcore::base::Thread::usleepFor(2 * ONE_SECOND);

				serial_start(this->serial);

			}
			catch (const char *msg)
			{
				cerr << "Serial error : " << msg << endl;
			}
		}

		void SerialHandler::tearDown()
		{
			if (serialBehaviour.compare("out") == 0)
			{
				protocol_data d_motor;
				d_motor.id = ID_OUT_MOTOR;
				d_motor.value = MOTOR_IDLE;
				d_motor.sub_id = NO_DATA;

				serial_send(this->serial, d_motor);

				protocol_data d_servo;
				d_servo.id = ID_OUT_SERVO;
				d_servo.value = STRAIGHT_DEGREES;
				d_servo.sub_id = NO_DATA;

				serial_send(this->serial, d_servo);
			}

			odcore::base::Thread::usleepFor(2 * ONE_SECOND);

			serial_stop(this->serial);
			serial_free(this->serial);
		}

		void SerialHandler::nextContainer(Container &c)
		{
			int verif = serialBehaviour.compare("in") == 0;
			cerr << "New CONTAINER : " << serialBehaviour << " " << verif << endl;

			if (serialBehaviour.compare("out") == 0)
			{
				if (c.getDataType() == AutomotiveMSG::ID())
				{
					const AutomotiveMSG automotiveMSG =
							c.getData<AutomotiveMSG>();

					bool _brake = automotiveMSG.getBrakeLights();
					int _lights = automotiveMSG.getLights();
					int arduinoAngle = STRAIGHT_DEGREES;
					int speed = MOTOR_IDLE;

					if (!_brake)
					{
						double angle = automotiveMSG.getSteeringWheelAngle();

						arduinoAngle = STRAIGHT_DEGREES + (angle * (MAX_DEGREES / _PI));
						if (arduinoAngle < MIN_DEGREES)
						{
							arduinoAngle = MIN_DEGREES;
						}
						else if (arduinoAngle > MAX_DEGREES)
						{
							arduinoAngle = MAX_DEGREES;
						}

						speed = automotiveMSG.getSpeed();
					}
					else
					{
						speed = MOTOR_IDLE;
						arduinoAngle = STRAIGHT_DEGREES;

//						if (_brake != this->brake)
//						{
						this->brake = _brake;

						protocol_data d_brake;
						d_brake.id = ID_OUT_LIGHTS;
						d_brake.value = NO_DATA;
						d_brake.sub_id = ID_OUT_BRAKE;

						serial_send(this->serial, d_brake);
//						}
					}
					//TODO lights ???????
					this->lights = _lights;
					protocol_data d_lights;
					d_lights.id = ID_OUT_LIGHTS;
					d_lights.value = NO_DATA;
					d_lights.sub_id = this->lights;

					d_lights = d_lights;

					//serial_send(this->serial, d_lights);

					if (this->motor != speed)
					{
						this->motor = speed;
						protocol_data d_motor;
						d_motor.id = ID_OUT_MOTOR;
						d_motor.value = this->motor;
						d_motor.sub_id = NO_DATA;

						serial_send(this->serial, d_motor);
					}

					if (this->servo != arduinoAngle)
					{
						this->servo = arduinoAngle;
						protocol_data d_servo;
						d_servo.id = ID_OUT_SERVO;
						d_servo.value = this->servo;
						d_servo.sub_id = NO_DATA;

						serial_send(this->serial, d_servo);
					}
				}

			}
			else if (serialBehaviour.compare("in") == 0)
			{
				cerr << "IN : " << endl;
				if (c.getDataType() == ResetMSG::ID())
				{
					Container resetContainer = c.getData<ResetMSG>();
					const ResetMSG resetMSG = resetContainer.getData<ResetMSG>();
					if (resetMSG.getResetOdometer())
					{
						odometerCounter = 0;
						km = 0;
					}
				}

				isSensorValues = false;
				raw_sensors[ID_IN_ULTRASONIC_CENTER].clear();
				raw_sensors[ID_IN_ULTRASONIC_CENTER_R].clear();
				raw_sensors[ID_IN_ULTRASONIC_SIDE_FRONT].clear();
				raw_sensors[ID_IN_ULTRASONIC_SIDE_BACK].clear();
				raw_sensors[ID_IN_ULTRASONIC_BACK].clear();

				raw_sensors[ID_IN_YAW].clear();
				raw_sensors[ID_IN_ROLL].clear();
				raw_sensors[ID_IN_PITCH].clear();

				int pending = g_async_queue_length(serial->incoming_queue);
				protocol_data incoming;

				for (int i = 0; i < pending; i++)
				{
					//If data available in the queue filter it
					if (serial_receive(serial, &incoming))
					{
						filterData(incoming.id, incoming.value);
					}//end of filtering
				}

				//If sensor data available
				if (isSensorValues)
				{
					sensorBoardDataMedian(ID_IN_ULTRASONIC_CENTER, raw_sensors[ID_IN_ULTRASONIC_CENTER]);
					sensorBoardDataMedian(ID_IN_ULTRASONIC_CENTER_R, raw_sensors[ID_IN_ULTRASONIC_CENTER_R]);
					sensorBoardDataMedian(ID_IN_ULTRASONIC_SIDE_FRONT, raw_sensors[ID_IN_ULTRASONIC_SIDE_FRONT]);
					sensorBoardDataMedian(ID_IN_ULTRASONIC_SIDE_BACK, raw_sensors[ID_IN_ULTRASONIC_SIDE_BACK]);
					sensorBoardDataMedian(ID_IN_ULTRASONIC_BACK, raw_sensors[ID_IN_ULTRASONIC_BACK]);

					sensorBoardDataMedian(ID_IN_YAW, raw_sensors[ID_IN_YAW]);
					sensorBoardDataMedian(ID_IN_ROLL, raw_sensors[ID_IN_ROLL]);
					sensorBoardDataMedian(ID_IN_PITCH, raw_sensors[ID_IN_PITCH]);

					sendSensorBoardData(sensors);

				}//end
			}

		}

		void SerialHandler::filterData(int id, int value)
		{
			if (id >= IDS_MIN_RANGE && id <= IDS_MAX_RANGE)
			{
				if (_debug)
				{
					cout << ">>SENSOR ID " << id << " | >> VALUE " << value << endl;
				}
				if (id >= ID_IN_ULTRASONIC_CENTER && id <= ID_IN_ULTRASONIC_BACK)
				{
					if (value > US_MIN_RANGE && value < US_MAX_RANGE)
					{
						raw_sensors[id].push_back(value);
					}
					else
					{
						raw_sensors[id].push_back(NO_OBSTACLE);
					}
				}
				else if (id >= ID_IN_YAW && id <= ID_IN_PITCH)
				{
					if (value >= G_MIN_RANGE && value < G_MAX_RANGE)
					{
						raw_sensors[id].push_back(value);
					}
					else
					{
						raw_sensors[id].push_back(-1);
					}
				}
				else if (id >= ID_IN_BUTTON_LANE && id <= ID_IN_BUTTON_OVERTAKE)
				{
					if (value == 0 || value == 1)
					{
						sensors[id] = value;
						sendSensorBoardData(sensors); //send signal
					}
					else if (value == 2)
					{
						sensors[id] = value;
					}
				}
				else if (id == ID_IN_ENCODER_L)
				{
					odometerCounter += value;

					if (odometerCounter >= KM_IN_CM)
					{
						odometerCounter -= KM_IN_CM;
						km++;
					}

					sensors[id] = odometerCounter;
					sensors[ID_IN_KM] = km;
				}

				isSensorValues = true;
			}
		}

		void SerialHandler::sensorBoardDataMedian(int id, vector<int> sensorList)
		{
			if (sensorList.size() > 0)
			{
				sort(sensorList.begin(), sensorList.end());
				int med = (int) sensorList.size() / 2;
				sensors[id] = sensorList[med];
			}
		}

		void SerialHandler::sendSensorBoardData(map<uint32_t, double> sensor)
		{
			sbd.setMapOfSensors(sensor);
			Container c(sbd);
			getConference().send(c);

			isSensorValues = false;
		}

		void __on_read(uint8_t b)
		{
#ifdef DEBUG
			cout << ">> read " << (int) b << endl;
#endif
		}

		void __on_write(uint8_t b)
		{
#ifdef DEBUG
			cout << "<< write " << (int) b << endl;
#endif
		}
	}
} // carolocup::control
