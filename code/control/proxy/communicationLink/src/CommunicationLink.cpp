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

#include "CommunicationLink.h"

namespace carolocup
{
    namespace control
    {

        using namespace std;
        using namespace odcore::base;
        using namespace odcore;
        using namespace odcore::base::module;
        using namespace odcore::data;
        using namespace odcore::wrapper;
        using namespace odcore::data::dmcp;
        using namespace gap;

        CommunicationLink::CommunicationLink(const int &argc, char **argv)
                : DataTriggeredConferenceClientModule(argc, argv, "carolocup-communicationlink"),
                  communicationLinkMSG(),
                  laneFollowerMSG(),
                  overtakerMSG(),
                  parkerMSG(),
                  sensorsMSG()
                  //UDPMSG()
        {}

        CommunicationLink::~CommunicationLink()
        {}

        void CommunicationLink::setUp()
        {
            cout << "Starting CommunicationLink" << endl;

            // Get configuration data.
            KeyValueConfiguration kv = getKeyValueConfiguration();

            communicationLinkMSG.setStateLaneFollower(kv.getValue<int32_t>("global.communicationlink.functionlane"));
            communicationLinkMSG.setStateOvertaker(kv.getValue<int32_t>("global.communicationlink.functionovertake"));
            communicationLinkMSG.setStateParker(kv.getValue<int32_t>("global.communicationlink.functionpark"));
        }

        void CommunicationLink::tearDown()
        {
            cout << "Shutting down CommunicationLink" << endl;
        }

        void CommunicationLink::nextContainer(Container &c)
        {

            if (c.getDataType() == SensorsMSG::ID())
            {
                Container sensorBoardDataContainer = c.getData<SensorsMSG>();

                sensorsMSG = sensorBoardDataContainer.getData<SensorsMSG>();

                //TODO set sensor data to comm link msg

                Container container(communicationLinkMSG);
                // Send container.
                getConference().send(container);
            } else if (c.getDataType() == OvertakerMSG::ID())
            {
                Container overtakerMSGContainer = c.getData<OvertakerMSG>();
                overtakerMSG = overtakerMSGContainer.getData<OvertakerMSG>();

                //TODO set overtake to comm link msg

                Container container(communicationLinkMSG);
                // Send container.
                getConference().send(container);
            } else if (c.getDataType() == ParkerMSG::ID())
            {
                Container parkerMSGContainer = c.getData<ParkerMSG>();
                parkerMSG = parkerMSGContainer.getData<ParkerMSG>();

                //TODO set park to comm link msg

                Container container(communicationLinkMSG);
                // Send container.
                getConference().send(container);
            } else if (c.getDataType() == LaneFollowerMSG::ID())
            {
                Container laneFollowerMSGContainer = c.getData<LaneFollowerMSG>();
                laneFollowerMSG = laneFollowerMSGContainer.getData<LaneFollowerMSG>();

                //TODO set lane to comm link msg

                Container container(communicationLinkMSG);
                // Send container.
                getConference().send(container);
            }

            //TODO UDPMSG AND LIDAR
        }

    }
} // carolocup::control
