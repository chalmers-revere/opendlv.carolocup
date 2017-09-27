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
        {}

        CommunicationLink::~CommunicationLink()
        {}

        void CommunicationLink::setUp()
        {
            cout << "Starting CommunicationLink" << endl;

            // Get configuration data.
            KeyValueConfiguration kv = getKeyValueConfiguration();

            communicationLinkMSG.setStateLaneFollower(kv.getValue<int32_t>("communicationlink.functionlane"));
            int func2 = kv.getValue<int32_t>("communicationlink.function2");

            if (func2 == 1)
            {
                communicationLinkMSG.setStateOvertaker(0);
                communicationLinkMSG.setStateParker(1);
            } else if (func2 == 0)
            {
                communicationLinkMSG.setStateOvertaker(1);
                communicationLinkMSG.setStateParker(0);
            } else
            {
                communicationLinkMSG.setStateOvertaker(0);
                communicationLinkMSG.setStateParker(0);
            }
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

                communicationLinkMSG.setWheelEncoder(sensorsMSG.getTravelledDistance());

                communicationLinkMSG.setUltraSonicFrontCenter(
                        sensorsMSG.getValueForKey_MapOfDistances(ID_IN_ULTRASONIC_CENTER));

                communicationLinkMSG.setUltraSonicFrontRight(
                        sensorsMSG.getValueForKey_MapOfDistances(ID_IN_ULTRASONIC_SIDE_FRONT));

                communicationLinkMSG.setInfraredSideFront(
                        sensorsMSG.getValueForKey_MapOfDistances(ID_IN_INFRARED_SIDE_FRONT));

                communicationLinkMSG.setInfraredSideBack(
                        sensorsMSG.getValueForKey_MapOfDistances(ID_IN_INFRARED_SIDE_BACK));

                communicationLinkMSG.setInfraredBack(sensorsMSG.getValueForKey_MapOfDistances(ID_IN_INFRARED_BACK));

                Container container(communicationLinkMSG);
                // Send container.
                getConference().send(container);
            } else if (c.getDataType() == OvertakerMSG::ID())
            {
                Container overtakerMSGContainer = c.getData<OvertakerMSG>();
                overtakerMSG = overtakerMSGContainer.getData<OvertakerMSG>();


                Container container(communicationLinkMSG);
                // Send container.
                getConference().send(container);
            } else if (c.getDataType() == ParkerMSG::ID())
            {
                Container parkerMSGContainer = c.getData<ParkerMSG>();
                parkerMSG = parkerMSGContainer.getData<ParkerMSG>();

                Container container(communicationLinkMSG);
                // Send container.
                getConference().send(container);
            } else if (c.getDataType() == LaneFollowerMSG::ID())
            {
                Container laneFollowerMSGContainer = c.getData<LaneFollowerMSG>();
                laneFollowerMSG = laneFollowerMSGContainer.getData<LaneFollowerMSG>();

                communicationLinkMSG.setDrivingLane(laneFollowerMSG.getStateLane());
                communicationLinkMSG.setDistanceToRightLane(laneFollowerMSG.getDistanceToRightLane());
                communicationLinkMSG.setStop(laneFollowerMSG.getDanger());
                Container container(communicationLinkMSG);
                // Send container.
                getConference().send(container);
            }
            Container container(communicationLinkMSG);
            // Send container.
            getConference().send(container);
        }

    }
} // carolocup::control
