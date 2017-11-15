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
				  odometerCounter(0),
				  km(0),
				  isSensorValues(false),
				  _debug(false)
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
				cerr << "Usage: " << argv[0] << "SOURCE DESTINATION (arduino=one/arduino=two)" << endl;
			}
		}

		SerialHandler::~SerialHandler()
		{}

		void SerialHandler::setUp()
		{
			try
			{
				if (serialBehaviour.compare("arduino=one") == 0 || serialBehaviour.compare("arduino=two") == 0)
				{
					KeyValueConfiguration kv = getKeyValueConfiguration();

					_debug = kv.getValue<int32_t>("global.debug") == 1;

					if (_debug)
					{
#define DEBUG
					}

					if (serialBehaviour.compare("arduino=one") == 0)
					{
						SERIAL_PORT = kv.getValue<string>("global.serialhandler.one");
					}
					else if (serialBehaviour.compare("arduino=two") == 0)
					{
						SERIAL_PORT = kv.getValue<string>("global.serialhandler.two");
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
						serialBehaviour = "arduino=out";
					}
					else if (rb == 's')
					{
						serialBehaviour = "arduino=in";
					}

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

					bool brake = automotiveMSG.getBrakeLights();
					int lights = automotiveMSG.getLights();
					if (!brake)
					{
						double angle = automotiveMSG.getSteeringWheelAngle();

						int arduinoAngle = STRAIGHT_DEGREES + (angle * (MAX_DEGREES / _PI));
						if (arduinoAngle < MIN_DEGREES)
						{
							arduinoAngle = MIN_DEGREES;
						}
						else if (arduinoAngle > MAX_DEGREES)
						{
							arduinoAngle = MAX_DEGREES;
						}

						int speed = automotiveMSG.getSpeed();

						this->motor = speed;
						this->servo = arduinoAngle;

					}
					else
					{
						this->motor = MOTOR_IDLE;
						this->servo = STRAIGHT_DEGREES;

						protocol_data d_brake;
						d_brake.id = ID_OUT_BRAKE;
						d_brake.value = NO_DATA;

						serial_send(this->serial, d_brake);
					}

					protocol_data d_lights;
					d_lights.id = ID_OUT_INDICATORS;
					d_lights.value = lights;

					d_lights = d_lights;

//					serial_send(this->serial, d_lights);

					protocol_data d_motor;
					d_motor.id = ID_OUT_MOTOR;
					d_motor.value = this->motor;

					serial_send(this->serial, d_motor);

					protocol_data d_servo;
					d_servo.id = ID_OUT_SERVO;
					d_servo.value = this->servo;

					serial_send(this->serial, d_servo);
				}

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
					cout << ">> ID " << id << " | >> VALUE " << value << endl;
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
			cout << ">> read " << b << endl;
#endif
		}

		void __on_write(uint8_t b)
		{
#ifdef DEBUG
			cout << "<< write " << b << endl;
#endif
		}
	}
} // carolocup::control
