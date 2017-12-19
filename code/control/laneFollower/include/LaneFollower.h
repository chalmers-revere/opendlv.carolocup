/**
 * laneFollower - laneFollower code.
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

#ifndef CONTROL_LANEFOLLOWER_H
#define CONTROL_LANEFOLLOWER_H

#include "../../defines/defines.h"

#include "ImgProcess.h"
#include "PID.h"

#include <stdint.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <memory>
#include <math.h>
#include <unistd.h>
#include <algorithm>

#include <opendavinci/odcore/base/module/TimeTriggeredConferenceClientModule.h>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <opendavinci/odcore/base/KeyValueConfiguration.h>
#include <opendavinci/odcore/base/Lock.h>

#include "opendavinci/odcore/io/conference/ContainerConference.h"
#include <opendavinci/odcore/data/Container.h>
#include <opendavinci/odcore/data/TimeStamp.h>

#include <opendavinci/odcore/wrapper/SharedMemoryFactory.h>
#include <opendavinci/odcore/wrapper/SharedMemory.h>
#include <opendavinci/GeneratedHeaders_OpenDaVINCI.h>
#include <automotivedata/GeneratedHeaders_AutomotiveData.h>
#include "automotivedata/generated/automotive/VehicleData.h"
#include "automotivedata/generated/automotive/miniature/SensorBoardData.h"
#include "odvdcarolocupdatamodel/generated/gap/CommunicationLinkMSG.h"
#include "odvdcarolocupdatamodel/generated/gap/LaneFollowerMSG.h"
#include "odvdcarolocupdatamodel/generated/gap/AutomotiveMSG.h"

namespace carolocup
{
	namespace control
	{

		using namespace std;
		using namespace gap;
		using namespace cv;
		using namespace odcore::data::image;

/**
 * Time-triggered laneFollower.
 */
		class LaneFollower : public odcore::base::module::TimeTriggeredConferenceClientModule
		{
		private:
			LaneFollower(const LaneFollower & /*obj*/) = delete;

			LaneFollower &operator=(const LaneFollower & /*obj*/) = delete;

		public:
			/**
			 * Constructor.
			 *
			 * @param argc Number of command line arguments.
			 * @param argv Command line arguments.
			 */
			LaneFollower(const int &argc, char **argv);

			virtual ~LaneFollower();

		private:
			void setUp();

			void tearDown();

			odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode body();

			bool m_debug;
			bool Sim;
			int pid_tuning;

			odcore::data::TimeStamp m_previousTime;

			double m_eSum;
			double m_eOld;

			AutomotiveMSG m_vehicleControl;
			LaneFollowerMSG laneFollowerMSG;

			double p_gain;
			double i_gain;
			double d_gain;

			int _state, _isOvertaking;

			bool *stop;
			double stopCounter;

			enum StateMachine
			{
				IDLE,
				MOVING,
				RESUME,
				STOP_LINE,
				STOP,
				DANGER
			};

			enum TwiddleState
			{
				BEGIN,
				UPDATE_ERROR_1,
				UPDATE_ERROR_2,
				UPDATE_ERROR_3,
				LOOP,
				COMPLETE
			};

			StateMachine state, prevState;

			bool inRightLane;
			int *currentDistance;
			int _stop;

			double speed, steer;

			ImgProcess imgProcess;

			PID auto_pid;

			double param[3];
			double dp[3];

			double best_err;
			double err;
			double sum;

			int firstIteration, iteration;

			TwiddleState t_state;

			void laneFollower(double e);

			void state_machine();

			void sendToConference(bool sendMSG);

		};
	}
} // carolocup::control

#endif /*CONTROL_LANEFOLLOWER_H*/
