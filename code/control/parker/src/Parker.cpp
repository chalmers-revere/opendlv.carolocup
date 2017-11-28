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
				  m_vehicleControl(),
				  accumulatedEncoderData(0),
				  stageProgress(0),
				  state(Search)
		{}

		Parker::~Parker()
		{}

		void Parker::setUp()
		{

		}

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
				double ultrasonicSideFront=sensors.get(ID_IN_ULTRASONIC_SIDE_FRONT);
				double ultraSonicBack=sensors.get(ID_IN_ULTRASONIC_SIDE_BACK);
				uint16 distanceToRightLane=communicationLinkMSG.getDistanceToRightLane();
				map<uint32,double> lidarDistance=communicationLinkMSG.LidarDistance();
				map<uint32,double> lidarStrength=communicationLinkMSG.LidarStrength();
				
				if(this.state==Parking){	
					vector<double> stages = getDistanceForStages();
					double turningAngle=90;
					int8_t direction=2;
				
					if(getIdealWheelEncoder(sensors)<=stages.at(i)){
						if(i%2){	
							m_vehicleControl.setSteeringWheelAngle(0);
							m_vehicleControl.setSpeed(1);
							m_vehicleControl.setLights(ID_OUT_INDICATOR_LF);
							m_vehicleControl.setLights(ID_OUT_INDICATOR_BF);
						}else{
							m_vehicleControl.setSteeringWheelAngle(180);
							m_vehicleControl.setSpeed(-1);
							m_vehicleControl.setLights(ID_OUT_INDICATOR_RF);
							m_vehicleControl.setLights(ID_OUT_INDICATOR_RB);
						}
						accumulatedEncoderData+=getIdealWheelEncoder(sensors);		
					}else{
						resetEncoders();
						m_vehicleControl.setSteeringWheelAngle(90);
						m_vehicleControl.setSpeed(0);
						if(stages.size()==++stageProgress){
							this.stateProgress=Parked;
						}
					}
					
					
				}else{
					accumulatedEncoderData+=getIdealWheelEncoder(sensors);
					if(this.state==Search && ultrasonicSideFront >0){
						this.state=DiscoveredInitialObject;
					}else if(this.state==DiscoveredInitialObject && ultrasonicSideFront == 0){
						this.state==MeasuringParkingSpace;
						resetEncoders();
					}else if(this.state==MeasuringParkingSpace && ultrasonicSideFront>0){
						this.currentSpaceSize=getIdealWheelEncoder(sensors);
						this.state=Positioning;
						resetEncoders();
					}else if(this.state==Positioning && getIdealWheelEncoder(sensors)>=50){
						this.state=Parking;	
					}
				
				}

			}
		}
		
		
		
		
		double Parker::getIdealWheelEncoder(map<uint32,double> sensors){
			double leftEncoder=sensors.get(ID_IN_ENCODER_L);
			double rightEncoder=sensors.get(ID_IN_ENCODER_R);
			return (leftEncoder+rightEncoder)/2;
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
			stages.push_back(arcDistance);
			stages.push_back(arcDistance);
			
			
			return stages;
			
		}
		
		void Parker::resetEncoders(){
			const ParkerMSG parkerMSG;
			parker.setReset(1);
		}
		
		
		double Parker::getLaneWidth(){
			return 0;
		}
		
		
		




	}
} // carolocup::control
