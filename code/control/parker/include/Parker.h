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

#include "opendavinci/odcore/io/conference/ContainerConference.h"
#include "opendavinci/odcore/data/Container.h"

#include "opendavinci/GeneratedHeaders_OpenDaVINCI.h"

#include "opendavinci/odcore/base/module/DataTriggeredConferenceClientModule.h"
#include "automotivedata/generated/automotive/miniature/SensorBoardData.h"
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
            void setUp();

            void tearDown();

            virtual void nextContainer(Container &c);

            void parkingFinder();

            void parallelPark();

            void setParkingState(int state);

            void setParkingType(int type);

            double adjDistCalculation(double start);

            bool obstacleDetection(int i, int id);

            void park();

            void unpark();

            void sendParkerMSG();

            AutomotiveMSG vc;

            double parkingSpace;

            bool IRRObstacle;

            bool USFObstacle;

            bool IRFRObstacle;

            bool IRRRObstacle;

            double odometer;

            double usFront;

            double usFrontRight;

            double irFrontRight;

            double irRear;

            double irRearRight;

            int parkingState;

            int parkingType;

            int parkingCounter;

            int parkingStart;

            double backDist;

            double backStart;

            double backEnd;

            double adjDist;

            bool isParking;

            double counterS;
            double counterO;
            int counter, _state;
            bool parked;
            double timer;
            double turnCount;
            CommunicationLinkMSG communicationLinkMSG;
            int _stop;
        };
    }
} // carolocup::control

#endif /*CONTROL_PARKER_H*/
