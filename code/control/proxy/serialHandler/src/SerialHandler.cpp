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
				  sensors()
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
				if (serialBehaviour.compare("arduino=in") || serialBehaviour.compare("arduino=out"))
				{
					KeyValueConfiguration kv = getKeyValueConfiguration();

					if (serialBehaviour.compare("arduino=in"))
					{
						SERIAL_PORT = kv.getValue<string>("global.serialhandler.sensors");
					}
					else if (serialBehaviour.compare("arduino=out"))
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

					//serial_handshake(this->serial, '\n');

					odcore::base::Thread::usleepFor(5 * ONE_SECOND);

					//TODO send first data

					//serial_send(this->serial, d_motor);
					//serial_send(this->serial, d_servo);

					odcore::base::Thread::usleepFor(2 * ONE_SECOND);

					serial_start(this->serial);

					//serial_ = this->serial;
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
			if (serialBehaviour.compare("arduino=out"))
			{
				//TODO send stop data

				//serial_send(this->serial, d_motor);
				//serial_send(this->serial, d_servo);

			}

			odcore::base::Thread::usleepFor(2 * ONE_SECOND);

			serial_stop(this->serial);
			serial_free(this->serial);
		}

		void SerialHandler::nextContainer(Container &c)
		{
			if (serialBehaviour.compare("arduino=out") == 0)
			{
				cout << "<< out " << endl;
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
				cout << "<< in " << endl;
				int pending = g_async_queue_length(serial->incoming_queue);
				protocol_data incoming;
				//TODO clear every single vector

				for (int i = 0; i < pending; i++)
				{
					if (serial_receive(serial, &incoming))
					{
						filterData(incoming.id, incoming.value);
						cout << ">> read " << incoming.id << " | >> read " << incoming.value <<endl;
					}

					//TODO sort and take the median

					//isSensorValues = true;
				}

//				if (isSensorValues)
//				{
//					sendSensorBoardData(sensors);
//				}
			}

		}

		void SerialHandler::filterData(int id, int value)
		{
			//TODO filter incomming data
			id=id;
			value =value;
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
			cout << "<< write " << (int) b << endl;
		}
	}
} // carolocup::control
