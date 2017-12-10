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
		using namespace odcore::data::image;
		using namespace odcore::wrapper;
		using namespace odcore::data::dmcp;

		Parker::Parker(const int &argc, char **argv)
				: DataTriggeredConferenceClientModule(argc, argv, "carolocup-parker"),
				  m_simulator(false),
				  state(Parking), //Testing
				  currentSpaceSize(0),
				  m_vehicleControl(),
					m_parker(),
					m_sensors(),
				  accumulatedEncoderData(0),
				  stageProgress(0),
				  isParking(false),
				  m_debug(false)

		{}

		Parker::~Parker()
		{}

		void Parker::setUp()
		{
			KeyValueConfiguration kv = getKeyValueConfiguration();
			m_debug = kv.getValue<int32_t>("global.debug") == 1;

		}

		void Parker::tearDown()
		{}

		void Parker::nextContainer(Container &c)
		{
			cout << "before first if" << endl;
			if (c.getDataType() == CommunicationLinkMSG::ID())
			{
				cout << "first if" << endl;
				//Container communicationLinkContainer = c.getData<CommunicationLinkMSG>();

				//const CommunicationLinkMSG communicationLinkMSG = communicationLinkContainer.getData<CommunicationLinkMSG>();
				//map<uint32_t, double> sensors =CommunicationLinkMSG::sensors();


				const CommunicationLinkMSG communicationLinkMSG = c.getData<CommunicationLinkMSG>();
				map<unsigned int, double> sensors = communicationLinkMSG.getMapOfSensors();

				//double ultrasonicSideBack=sensors[ID_IN_ULTRASONIC_BACK];
				//double ultrasonicSideFront=sensors[ID_IN_ULTRASONIC_SIDE_FRONT];
				//double ultraSonicBack=sensors.get(ID_IN_ULTRASONIC_SIDE_BACK);
				//suff<
				//uint16_t distanceToRightLane=communicationLinkMSG.getDistanceToRightLane();
				//map<uint32_t,double> lidarDistance=communicationLinkMSG.LidarDistance();
				//map<uint32_t,double> lidarStrength=communicationLinkMSG.LidarStrength();

				if(this->state==Parking){
					vector<double> stages = getDistanceForStages();
					//double turningAngle=90;
					//int8_t direction=2;

					if(getIdealWheelEncoder(sensors)<=stages.at(stageProgress)){
						if(stageProgress%2){
							m_vehicleControl.setSteeringWheelAngle(0);
							m_vehicleControl.setSpeed(1);
							m_vehicleControl.setLights(ID_OUT_INDICATOR_LF);
							m_vehicleControl.setLights(ID_OUT_INDICATOR_LB);
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
						if(stages.size()==(unsigned)++stageProgress){
							this->state=Parked;
						}
					}

				}else{
					/*accumulatedEncoderData+=getIdealWheelEncoder(sensors);
					if(this->state==Search && ultrasonicSideFront >0){
						this->state=DiscoveredInitialObject;
					}else if(this->state==DiscoveredInitialObject && ultrasonicSideFront == 0){
						this->state==MeasuringParkingSpace;
						resetEncoders();
					}else if(this->state==MeasuringParkingSpace && ultrasonicSideFront>0){
						this->currentSpaceSize=getIdealWheelEncoder(sensors);
						this->state=Positioning;
						resetEncoders();
					}else if(this->state==Positioning && getIdealWheelEncoder(sensors)>=50){
						this->state=Parking;
					}*/

				}
				cout<< getIdealWheelEncoder(sensors)<<endl;
				cout<<" Car state "<<this->state<<endl;
				cout<<" Car stage "<<stageProgress<<endl;
				cout<<" distance travled "<< getIdealWheelEncoder(sensors)<<endl;

			}
		}




		double Parker::getIdealWheelEncoder(map<unsigned int,double> sensors){
			double leftEncoder=sensors[ID_IN_ENCODER_L];
			//return (leftEncoder+rightEncoder)/2;
			return leftEncoder;
		}

		vector<double> Parker::getDistanceForStages(){
			//KeyValueConfiguration kv = getKeyValueConfiguration();
			//double carWidth= kv.getValue<double>("global.car.width");
			//double carLength= kv.getValue<double>("global.car.length");
			double parkingWidth= 10;
			double laneWidth= 10;
			//double ackermannAngle= kv.getValue<double>("global.ackermannAngle");


			double circleDiameter=	laneWidth/2+parkingWidth/2;
			double circleCircumpherence=circleDiameter*3.14;
			double arcDistance=circleCircumpherence/8;

			vector<double> stages;
			stages.push_back(arcDistance);
			stages.push_back(arcDistance);


			return stages;

		}

		void Parker::resetEncoders(){
			//const ParkerMSG parkerMSG;
			//parker.setReset(1);
		}
		double Parker::getLaneWidth(){
			return 0;
		}







	}
} // carolocup::control
