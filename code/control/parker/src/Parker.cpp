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
				  m_simulator(false), //Set m_simulator to true if simulator is used and false otherwise.
				  m_vehicleControl()
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
				
				
				vector<double> stages=getDistanceForStages();
				double turningAngle=90;
				int8_t direction=2;
				for(int i=0;i<stages.length;i++){
					resetEncoders();
					while(getIdealWheelEncoder<=stages.get(i)*(1+i)){
						if(i%2){	
							m_vehicleControl.setSteeringWheelAngle(0);
							m_vehicleControl.setSpeed(1);
						}
						else{
							m_vehicleControl.setSteeringWheelAngle(180);
							m_vehicleControl.setSpeed(-1)
						}	
					}
				}
				m_vehicleControl.setSteeringWheelAngle(90);
				m_vehicleControl.setSpeed(0);

			}
		}
		
		double Parker::getIdealWheelEncoder(map<uint32,double> sensors){
			double leftEncoder=sensors.get(ID_IN_ENCODER_L);
			return leftEncoder;
			//double rightEncoder=sensors.get(ID_IN_ENCODER_R);
			//return (leftEncoder+rightEncoder)/2;
		}
		
		vector<double> Parker::getDistanceForStages(){
			KeyValueConfiguration kv = getKeyValueConfiguration();
			double carWidth= kv.getValue<double>("global.car.width");
			double carLength= kv.getValue<double>("global.car.length");
			double parkingWidth= kv.getValue<double>("global.parking.width");
			double laneWidth= kv.getValue<double>("global.lane.width");
			double ackermannAngle= kv.getValue<double>("global.ackermannAngle");
			
			
			double circleDiameter=	laneWidth/2+parkingWidth/2;
			double circleCircumpherence=circleDiameter*_Pi;
			double arcDistance=circleCircumpherence/8;
			
			vector<double> stages;
			stages.add(arcDistance);
			stages.add(arcDistance);
			
			return stages;
			
		}
		
		void Parker::resetEncoders(){
			const ParkerMSG parkerMSG;
			parker.setReset(1);
		}
		
		
		double Parker::getLaneWidth(){
			return 0;
		}
		
		
		

		vector<double> Parker::getFoundGaps() const {
			return m_foundGaps;
		}


	}
} // carolocup::control
