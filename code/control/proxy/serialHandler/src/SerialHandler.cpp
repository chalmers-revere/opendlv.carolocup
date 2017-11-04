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
		using namespace gap;

		SerialHandler::SerialHandler(const int &argc, char **argv)
				: DataTriggeredConferenceClientModule(argc, argv, "carolocup-serialhandler"),
				  serial(),
				  SERIAL_PORT("dummy"),
				  motor(90),
				  servo(90),
				  sbd(),
				  sensors(),
				  raw_sensors(),
				  steps(0),
				  odometerCounter(0),
				  km(0),
				  isSensorValues(false)
		{
			for (int i = 0; i < argc; ++i)
			{
				cout << argv[i] << endl; //the argument we are looking for is the i=3
			}

			if (argc >= 4)
			{
				serialBehaviour = argv[3];
			}
			else
			{
				cerr << "Usage: " << argv[0] << "SOURCE DESTINATION" << endl;
			}
		}

		SerialHandler::~SerialHandler()
		{}

		void SerialHandler::setUp()
		{
			try
			{
				if (serialBehaviour.compare("arduino=in") == 0 || serialBehaviour.compare("arduino=out") == 0)
				{
					KeyValueConfiguration kv = getKeyValueConfiguration();

					if (serialBehaviour.compare("arduino=in") == 0)
					{
						SERIAL_PORT = kv.getValue<string>("global.serialhandler.sensors");
					}
					else if (serialBehaviour.compare("arduino=out") == 0)
					{
						SERIAL_PORT = kv.getValue<string>("global.serialhandler.actuators");
					}

					const string _S_PORT = SERIAL_PORT;

					cerr << "Setting up serial handler to port " << SERIAL_PORT << endl;

					this->serial = serial_new();

					this->serial->on_write = &__on_write;
					this->serial->on_read = &__on_read;

					const char *_port = _S_PORT.c_str();
					serial_open(this->serial, _port, BAUD_RATE);

					serial_handshake(this->serial, '\n');

					odcore::base::Thread::usleepFor(5 * ONE_SECOND);

					if (serialBehaviour.compare("arduino=out") == 0)
					{
						protocol_data d_motor;
						d_motor.id = ID_OUT_MOTOR;
						d_motor.value = MOTOR_IDLE;

						serial_send(this->serial, d_motor);

						protocol_data d_servo;
						d_servo.id = ID_OUT_SERVO;
						d_servo.value = STRAIGHT_DEGREES;

						serial_send(this->serial, d_servo);
					}

					odcore::base::Thread::usleepFor(2 * ONE_SECOND);

					serial_start(this->serial);
				}
				else
				{
					cerr << "Wrong Usage!" << endl;
					throw "No Port Created!";
				}
			}
			catch (const char *msg)
			{
				cerr << "Serial error : " << msg << endl;
			}
		}

		void SerialHandler::tearDown()
		{
			if (serialBehaviour.compare("arduino=out") == 0)
			{
				protocol_data d_motor;
				d_motor.id = ID_OUT_MOTOR;
				d_motor.value = MOTOR_IDLE;

				serial_send(this->serial, d_motor);

				protocol_data d_servo;
				d_servo.id = ID_OUT_SERVO;
				d_servo.value = STRAIGHT_DEGREES;

				serial_send(this->serial, d_servo);
			}

			odcore::base::Thread::usleepFor(2 * ONE_SECOND);

			serial_stop(this->serial);
			serial_free(this->serial);
		}

		void SerialHandler::nextContainer(Container &c)
		{
			if (serialBehaviour.compare("arduino=out") == 0)
			{
				if (c.getDataType() == AutomotiveMSG::ID())
				{
					const AutomotiveMSG automotiveMSG =
							c.getData<AutomotiveMSG>();

					if (!automotiveMSG.getBrakeLights())
					{
						double angle = automotiveMSG.getSteeringWheelAngle();

						int arduinoAngle = 90 + (angle * (180 / _PI));
						if (arduinoAngle < 0)
						{
							arduinoAngle = 0;
						}
						else if (arduinoAngle > 180)
						{
							arduinoAngle = 180;
						}

						int speed = automotiveMSG.getSpeed();

						this->motor = speed;
						this->servo = arduinoAngle;

					}
					else
					{
						this->motor = 90;
						this->servo = 90;
					}
				}


				protocol_data d_motor;
				d_motor.id = ID_OUT_MOTOR;
				d_motor.value = this->motor;

				serial_send(this->serial, d_motor);

				protocol_data d_servo;
				d_servo.id = ID_OUT_SERVO;
				d_servo.value = this->servo;

				serial_send(this->serial, d_servo);

				//TODO send rest of data (lights)
			}
			else if (serialBehaviour.compare("arduino=in") == 0)
			{
				c = c;
				isSensorValues = false;
				raw_sensors[ID_IN_ULTRASONIC_CENTER].clear();
				raw_sensors[ID_IN_ULTRASONIC_CENTER_R].clear();
				raw_sensors[ID_IN_ULTRASONIC_SIDE_FRONT].clear();
				raw_sensors[ID_IN_ULTRASONIC_SIDE_BACK].clear();
				raw_sensors[ID_IN_ULTRASONIC_BACK].clear();

				raw_sensors[ID_IN_GX].clear();
				raw_sensors[ID_IN_GY].clear();
				raw_sensors[ID_IN_GZ].clear();

				raw_sensors[ID_IN_AX].clear();
				raw_sensors[ID_IN_AY].clear();
				raw_sensors[ID_IN_AZ].clear();

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

					sensorBoardDataMedian(ID_IN_GX, raw_sensors[ID_IN_GX]);
					sensorBoardDataMedian(ID_IN_GY, raw_sensors[ID_IN_GY]);
					sensorBoardDataMedian(ID_IN_GZ, raw_sensors[ID_IN_GZ]);

					sensorBoardDataMedian(ID_IN_AX, raw_sensors[ID_IN_AX]);
					sensorBoardDataMedian(ID_IN_AY, raw_sensors[ID_IN_AY]);
					sensorBoardDataMedian(ID_IN_AZ, raw_sensors[ID_IN_AZ]);

					sendSensorBoardData(sensors);
				}//end
			}

		}

		void SerialHandler::filterData(int id, int value)
		{
			if (id >= IDS_MIN_RANGE && id <= IDS_MAX_RANGE)
			{
				cout << ">> ID " << id << " | >> VALUE " << value << endl;
				if (id >= ID_IN_ULTRASONIC_CENTER && id <= ID_IN_ULTRASONIC_BACK)
				{
					if (value > US_MIN_RANGE && value < US_MAX_RANGE) //TODO set values to define list
					{
						raw_sensors[id].push_back(value);
					}
					else
					{
						raw_sensors[id].push_back(NO_OBSTACLE);
					}
				}
				else if (id >= ID_IN_GX && id <= ID_IN_GZ)
				{
					if (value >= G_MIN_RANGE && value < G_MAX_RANGE) //TODO set values to define list
					{
						raw_sensors[id].push_back(value);
					}
					else
					{
						raw_sensors[id].push_back(-1);
					}
				}
				else if (id >= ID_IN_AX && id <= ID_IN_AZ)
				{
					if (value >= A_MIN_RANGE && value < A_MAX_RANGE) //TODO set values to define list
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
					if (value == 1 || value == 0) {
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
				else if (id == ID_IN_STEP)
				{
					steps += value;

					sensors[id] = steps;
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
//			sbd.setMapOfDistances(sensor);
//			Container c(sbd);
//			getConference().send(c);
//			isSensorValues = false;
			sensor = sensor;
		}

		void __on_read(uint8_t b)
		{
			b = b;
			//cout << ">> read " << (int) b << endl;
		}

		void __on_write(uint8_t b)
		{
			b = b;
			//cout << "<< write " << (int) b << endl;
		}
	}
} // carolocup::control
