/**
 * Overtaker - Overtaker code.
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

#ifndef CONTROL_OVERTAKER_H
#define CONTROL_OVERTAKER_H

#include "../../defines/defines.h"

#include <iostream>
#include <stdint.h>
#include <memory>
#include <math.h>
#include <cstdio>
#include <cmath>

#include <opendavinci/odcore/base/KeyValueConfiguration.h>
#include <opendavinci/odcore/base/Lock.h>
#include <opendavinci/odcore/base/module/DataTriggeredConferenceClientModule.h>
#include <opendavinci/odcore/io/conference/ContainerConference.h>

#include <opendavinci/odcore/data/Container.h>
#include <opendavinci/odcore/data/TimeStamp.h>
#include <opendavinci/odcore/wrapper/SharedMemoryFactory.h>
#include <opendavinci/odcore/wrapper/SharedMemory.h>
#include <opendavinci/GeneratedHeaders_OpenDaVINCI.h>

#include "odvdcarolocupdatamodel/generated/gap/CommunicationLinkMSG.h"
#include "odvdcarolocupdatamodel/generated/gap/OvertakerMSG.h"
#include "odvdcarolocupdatamodel/generated/gap/AutomotiveMSG.h"


namespace carolocup
{
    namespace control
    {

        using namespace std;
        using namespace gap;
        using namespace odcore::base::module;
        using namespace odcore::data;

/**
 * Time-triggered Overtaker.
 */
        class Overtaker : public odcore::base::module::DataTriggeredConferenceClientModule
        {
        private:
            Overtaker(const Overtaker & /*obj*/) = delete;

            Overtaker &operator=(const Overtaker & /*obj*/) = delete;

        public:
            /**
             * Constructor.
             *
             * @param argc Number of command line arguments.
             * @param argv Command line arguments.
             */
            Overtaker(const int &argc, char **argv);

            virtual ~Overtaker();

        private:
            void setUp();

            void tearDown();

            virtual void nextContainer(Container &c);

            void measuringMachine();

            enum StateMachine {
                FIND_OBJECT,
                FIND_OBJECT_PLAUSIBLE,
                HAVE_BOTH_IR,
                HAVE_NO_READING,
                KEEP_TURN_RIGHT,
                KEEP_TURN_RIGHT_END,
                ADJUST_TO_STRAIGHT,
                DISABLE
            };

            AutomotiveMSG m_vehicleControl;

            bool Sim;

            int _state;

            OvertakerMSG overtakerMSG;

            const double OVERTAKING_DISTANCE;
            const double HEADING_PARALLEL;

            const double SPEED_CAR;
            const double TURN_ANGLE_CAR_LEFT;
            const double TURN_ANGLE_CAR_RIGHT;
            const double STRAIGHT_ANGLE_CAR;

            double IR_RR;
            double IR_FR;
            double IR_BACK;
            double US_C;
            double US_R;

            int odometerReal;
            int oldOdometer;
            int odo;

            int us_c_old;
            int _us_c_old;

            StateMachine stage;

            double distanceToObstacle;
            double distanceToObstacleOld;

            const int OBJECT_PLAUSIBLE_COUNT;
            int objectPlausibleCount;
            int objectDangerCount;

            int _stop;

            int valid_us;

            int park_state;

            int flag;
        };
    }
} // carolocup::control

#endif /*CONTROL_OVERTAKER_H*/
