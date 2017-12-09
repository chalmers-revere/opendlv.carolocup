/**
 * Parker - Parker code.
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

#ifndef CONTROL_PARKER_H
#define CONTROL_PARKER_H

#include "../../defines/defines.h"

#include <cstdio>
#include <cmath>
#include <iostream>
#include <math.h>
#include <cstring>

#include "opendavinci/odcore/io/conference/ContainerConference.h"
#include "opendavinci/odcore/data/Container.h"

#include "opendavinci/odcore/base/KeyValueConfiguration.h"
#include "opendavinci/odcore/data/TimeStamp.h"

#include <opendavinci/odcore/base/Thread.h>
#include "opendavinci/GeneratedHeaders_OpenDaVINCI.h"

#include "opendavinci/odcore/base/module/DataTriggeredConferenceClientModule.h"

#include "odvdcarolocupdatamodel/generated/gap/CommunicationLinkMSG.h"
#include "odvdcarolocupdatamodel/generated/gap/ParkerMSG.h"
#include "odvdcarolocupdatamodel/generated/gap/AutomotiveMSG.h"

namespace carolocup
{
	namespace control
	{

		using namespace std;
		using namespace odcore::base;
		using namespace odcore::base::module;
		using namespace odcore::data;
		using namespace gap;

/**
 * Time-triggered Parker.
 */
		class Parker : public odcore::base::module::DataTriggeredConferenceClientModule
		{
		private:
			Parker(const Parker & /*obj*/) = delete;

			Parker &operator=(const Parker & /*obj*/) = delete;

		public:
			/**
			 * Constructor.
			 *
			 * @param argc Number of command line arguments.
			 * @param argv Command line arguments.
			 */
			Parker(const int &argc, char **argv);

			virtual ~Parker();

		private:
			
			bool m_simulator;

			enum CarState
			{
				Search,
				DiscoveredInitialObject,
				MeasuringParkingSpace,
				Positioning,
				Parking,
				Parked
				
			};

			CarState state;
			double currentSpaceSize;

			AutomotiveMSG m_vehicleControl;
			long double accumulatedEncoderData;
			int stageProgress;
			
			bool isParking;
			
			
			
			
			void setUp();

			void tearDown();

			virtual void nextContainer(Container &c);

			/**
 			 * This method returns the gaps found during the parking process.
			 *
 			 * return Gaps found during the parking process.
 			 */
			
			double getIdealWheelEncoder(map<uint32_t,double>);
			void resetEncoders();
			vector<double> getDistanceForStages();
			
			double getLaneWidth();
			
			char readOdometer();
			int readSensorData(int sensorId);

			void sendMotionData(double steeringAngle, int speed);

			
			

		};
	}
} // carolocup::control

#endif /*CONTROL_PARKER_H*/
