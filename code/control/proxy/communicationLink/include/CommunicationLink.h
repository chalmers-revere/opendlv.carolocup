/**
 * CommunicationLink - CommunicationLink code.
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

#ifndef CONTROL_COMMUNICATIONLINK_H
#define CONTROL_COMMUNICATIONLINK_H

#include "../../../defines/defines.h"
#include <opendavinci/odcore/base/module/DataTriggeredConferenceClientModule.h>
#include <opendavinci/odcore/data/Container.h>
#include <opendavinci/odcore/data/TimeStamp.h>

#include <opendavinci/odcore/wrapper/SharedMemory.h>
#include <opendavinci/odcore/wrapper/SharedMemoryFactory.h>

#include <opendavinci/odcore/base/Thread.h>
#include "odvdcarolocupdatamodel/generated/gap/CommunicationLinkMSG.h"
#include "odvdcarolocupdatamodel/generated/gap/LaneFollowerMSG.h"
#include "odvdcarolocupdatamodel/generated/gap/OvertakerMSG.h"
#include "odvdcarolocupdatamodel/generated/gap/ParkerMSG.h"
#include "odvdcarolocupdatamodel/generated/gap/SensorsMSG.h"
#include "odvdcarolocupdatamodel/generated/gap/LIDARMSG.h"
#include "odvdcarolocupdatamodel/generated/gap/UdpMSG.h"
#include "odvdcarolocupdatamodel/generated/gap/GyroMSG.h"
#include <opendavinci/odcore/base/KeyValueConfiguration.h>

#include <iostream>
#include <memory>
#include <stdint.h>
#include <string>

namespace carolocup
{
    namespace control
    {

        using namespace std;
        using namespace odcore;
        using namespace odcore::base::module;
        using namespace odcore::data;
        using namespace odcore::wrapper;
        using namespace gap;

/**
 * Time-triggered CommunicationLink.
 */
        class CommunicationLink : public odcore::base::module::DataTriggeredConferenceClientModule
        {
        private:
            CommunicationLink(const CommunicationLink & /*obj*/) = delete;

            CommunicationLink &operator=(const CommunicationLink & /*obj*/) = delete;

        public:
            /**
             * Constructor.
             *
             * @param argc Number of command line arguments.
             * @param argv Command line arguments.
             */
            CommunicationLink(const int &argc, char **argv);

            virtual ~CommunicationLink();

        private:
            void setUp();

            void tearDown();

            virtual void nextContainer(Container &c);

            CommunicationLinkMSG communicationLinkMSG;
        };
    }
} // carolocup::control

#endif /*CONTROL_COMMUNICATIONLINK_H*/
