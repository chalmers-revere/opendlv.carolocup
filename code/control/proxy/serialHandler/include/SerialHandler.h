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

#ifndef CONTROL_SERIALHANDLER_H
#define CONTROL_SERIALHANDLER_H

#include "../../../defines/defines.h"

#include <opendavinci/odcore/base/module/DataTriggeredConferenceClientModule.h>

#include <opendavinci/GeneratedHeaders_OpenDaVINCI.h>
#include <automotivedata/GeneratedHeaders_AutomotiveData.h>
#include "automotivedata/generated/automotive/VehicleData.h"
#include "automotivedata/generated/automotive/miniature/SensorBoardData.h"
#include "odvdcarolocupdatamodel/generated/gap/AutomotiveMSG.h"
#include "odvdcarolocupdatamodel/generated/gap/SensorsMSG.h"
#include "odvdcarolocupdatamodel/generated/gap/ResetMSG.h"

#include <opendavinci/odcore/data/Container.h>
#include <opendavinci/odcore/data/TimeStamp.h>
#include <opendavinci/odcore/base/Thread.h>

#include <opendavinci/odcore/base/Service.h>

#include <opendavinci/odcore/wrapper/SerialPort.h>
#include <opendavinci/odcore/wrapper/SerialPortFactory.h>
#include <opendavinci/odcore/wrapper/SharedMemory.h>
#include <opendavinci/odcore/wrapper/SharedMemoryFactory.h>

#include <opendavinci/odcore/base/KeyValueConfiguration.h>


#include <iostream>
#include <memory>
#include <stdint.h>
#include <string>
#include <vector>
#include <cctype>
#include <algorithm>

#include "serial.h"
#include "Protocol.h"

namespace carolocup
{
	namespace control
	{

		using namespace std;
		using namespace odcore;
		using namespace odcore::base::module;
		using namespace odcore::data;
		using namespace odcore::wrapper;
		using namespace automotive;
		using namespace automotive::miniature;
		using namespace gap;

		void __on_read(uint8_t b);

		void __on_write(uint8_t b);

/**
 * DataTriggered SerialHandler.
 */
		class SerialHandler : public odcore::base::module::DataTriggeredConferenceClientModule
		{
		private:
			SerialHandler(const SerialHandler & /*obj*/) = delete;

			SerialHandler &operator=(const SerialHandler & /*obj*/) = delete;

		public:
			/**
			 * Constructor.
			 *
			 * @param argc Number of command line arguments.
			 * @param argv Command line arguments.
			 */
			SerialHandler(const int &argc, char **argv);

			virtual ~SerialHandler();

		private:
			void setUp();

			void tearDown();

			virtual void nextContainer(Container &c);

			void filterData(int id, int value);

			void sensorBoardDataMedian(int id, vector<int> sensorList);

			void sendSensorBoardData(std::map<uint32_t, double> sensor);

			serial_state *serial;

			int motor;
			int servo;
			int lights;
			bool brake;

			SensorsMSG sbd;

			map<uint32_t, double> sensors;

			map<int, vector<int>> raw_sensors;

			int odometerCounter;

			int km;

			bool isSensorValues;

			bool _debug;

			string serialBehaviour;

			string SERIAL_PORT;
		};
	}
} // carolocup::control

#endif /*CONTROL_SERIALHANDLER_H*/
