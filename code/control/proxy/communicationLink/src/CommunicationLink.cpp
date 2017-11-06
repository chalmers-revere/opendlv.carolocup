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
				  communicationLinkMSG()
		{}

		CommunicationLink::~CommunicationLink()
		{}

		void CommunicationLink::setUp()
		{
			cout << "Starting CommunicationLink" << endl;

			// Get configuration data.
			//KeyValueConfiguration kv = getKeyValueConfiguration();

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
				SensorsMSG sensorsMSG = sensorBoardDataContainer.getData<SensorsMSG>();

				communicationLinkMSG.setMapOfSensors(sensorsMSG.getMapOfSensors());
				if (sensorsMSG.getValueForKey_MapOfSensors(ID_IN_BUTTON_LANE) < 2) {
					communicationLinkMSG.setStateLaneFollower(sensorsMSG.getValueForKey_MapOfSensors(ID_IN_BUTTON_LANE));
				}

				if (sensorsMSG.getValueForKey_MapOfSensors(ID_IN_BUTTON_PARK) < 2) {
					communicationLinkMSG.setStateParker(sensorsMSG.getValueForKey_MapOfSensors(ID_IN_BUTTON_PARK));
				}

				if (sensorsMSG.getValueForKey_MapOfSensors(ID_IN_BUTTON_OVERTAKE) < 2) {
					communicationLinkMSG.setStateOvertaker(sensorsMSG.getValueForKey_MapOfSensors(ID_IN_BUTTON_OVERTAKE));
				}
			}
			else if (c.getDataType() == OvertakerMSG::ID())
			{
				Container overtakerMSGContainer = c.getData<OvertakerMSG>();
				OvertakerMSG overtakerMSG = overtakerMSGContainer.getData<OvertakerMSG>();

				communicationLinkMSG.setStateLaneFollower(overtakerMSG.getStateStop());
				communicationLinkMSG.setStateParker(0);
				communicationLinkMSG.setDrivingLane(overtakerMSG.getStateLane());

			}
			else if (c.getDataType() == ParkerMSG::ID())
			{
				Container parkerMSGContainer = c.getData<ParkerMSG>();
				ParkerMSG parkerMSG = parkerMSGContainer.getData<ParkerMSG>();

				communicationLinkMSG.setStateLaneFollower(parkerMSG.getStateStop());
				communicationLinkMSG.setStateOvertaker(0);
			}
			else if (c.getDataType() == LaneFollowerMSG::ID())
			{
				Container laneFollowerMSGContainer = c.getData<LaneFollowerMSG>();
				LaneFollowerMSG laneFollowerMSG = laneFollowerMSGContainer.getData<LaneFollowerMSG>();

				communicationLinkMSG.setStateDanger(laneFollowerMSG.getStateDanger());
				communicationLinkMSG.setDrivingLane(laneFollowerMSG.getStateLane());
				communicationLinkMSG.setDistanceToRightLane(laneFollowerMSG.getDistanceToRightLane());

			}
			else if (c.getDataType() == LIDARMSG::ID())
			{
				Container LIDARMSGContainer = c.getData<LIDARMSG>();
				LIDARMSG lidarMSG = LIDARMSGContainer.getData<LIDARMSG>();

				communicationLinkMSG.setMapOfLidarDistances(lidarMSG.getMapOfLidarDistances());
				communicationLinkMSG.setMapOfLidarStrength(lidarMSG.getMapOfLidarStrength());
			}
			else if (c.getDataType() == UdpMSG::ID())
			{
				Container udpMSGContainer = c.getData<UdpMSG>();
				UdpMSG udpMSG = udpMSGContainer.getData<UdpMSG>();

				communicationLinkMSG.setStateLaneFollower(udpMSG.getStateFunctionLane());
				communicationLinkMSG.setStateOvertaker(udpMSG.getStateFunctionOvertaker());
				communicationLinkMSG.setStateParker(udpMSG.getStateFunctionParker());
			}

			Container container(communicationLinkMSG);
			// Send container.
			getConference().send(container);
		}

	}
} // carolocup::control
