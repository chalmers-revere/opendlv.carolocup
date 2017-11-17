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

#include "Parker.h"

namespace carolocup
{
	namespace control
	{

		using namespace std;
		using namespace odcore::base;
		using namespace odcore::data;
		using namespace gap;

		Parker::Parker(const int &argc, char **argv)
				: DataTriggeredConferenceClientModule(argc, argv, "carolocup-parker"),
				  state(Search),
				  currentSpaceSize(0),
				  m_foundGaps(),
				  m_simulator(false) //Set m_simulator to true if simulator is used and false otherwise.
		{}

		Parker::~Parker()
		{}

		void Parker::setUp()
		{}

		void Parker::tearDown()
		{}

		void Parker::nextContainer(Container &c)
		{
			if (c.getDataType() == CommunicationLinkMSG::ID())
			{
				Container communicationLinkContainer = c.getData<CommunicationLinkMSG>();
				const CommunicationLinkMSG communicationLinkMSG = communicationLinkContainer.getData<CommunicationLinkMSG>();
				map<uint32, double> sensors =communicationLinkMSG.getSensors;
				double ultrasonicSideBack=sensors.get(ID_IN_ULTRASONIC_BACK);
				double ultrasonicSideFront=sensors.get(ID_IN_ULTRASONIC_FRONT);
				double ultraSonicBack=sensors.get(ID_IN_ULTRASONIC_BACK);
				uint16 distanceToRightLane=communicationLinkMSG.getDistanceToRightLane();
				map<uint32,double> lidarDistance=communicationLinkMSG.LidarDistance();
				map<uint32,double> lidarStrength=communicationLinkMSG.LidarStrength();

			}
		}
		
		double Parker::getIdealWheelTrajectory(map<uint32,double> sensors){
			double leftEncoder=sensors.get(ID_IN_ENCODER_L);
			double rightEncoder=sensors.get(ID_IN_ENCODER_R);
			return 0;
		}
		
		double Parker::getLaneWidth(){
			return 0;
		
		}
		
		
		

		vector<double> Parker::getFoundGaps() const {
			return m_foundGaps;
		}


	}
} // carolocup::control
