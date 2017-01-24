/**
 * proxy - Sample application to encapsulate HW/SW interfacing with embedded systems.
 * Copyright (C) 2012 - 2015 Christian Berger
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

#include <ctype.h>
#include <cstring>
#include <cmath>
#include <iostream>
#include <memory>

#include "opendavinci/odcore/base/KeyValueConfiguration.h"
#include "opendavinci/odcore/data/Container.h"
#include "opendavinci/odcore/data/TimeStamp.h"

#include "opendavinci/odcore/serialization/ProtoSerializerVisitor.h"
#include "automotivedata/GeneratedHeaders_AutomotiveData.h"
#include "opendavinci/odcore/serialization/ProtoDeserializerVisitor.h"

#include <opendavinci/odcore/wrapper/SerialPort.h>
#include <opendavinci/odcore/wrapper/SerialPortFactory.h>


#include "OpenCVCamera.h"

#ifdef HAVE_UEYE
    #include "uEyeCamera.h"
#endif

#include "Proxy.h"

namespace automotive {
    namespace miniature {

        using namespace std;
        using namespace odcore::base;
        using namespace odcore::data;
        using namespace odcore::serialization;
        using namespace odtools::recorder;

        const string SERIAL_PORT = "/dev/ttyACM0";
        const uint32_t BAUD_RATE = 115200;
        char flagEND   = 19; 
        char flagESC   = 125; 
        int  flagXOR   = 32;
        int loopCounter=0;
        //int wheelencoder;
        stringstream netstring;

        Proxy::Proxy(const int32_t &argc, char **argv) :
            TimeTriggeredConferenceClientModule(argc, argv, "proxy"),
            serial(odcore::wrapper::SerialPortFactory::createSerialPort(SERIAL_PORT, BAUD_RATE)),
            m_recorder(),
            m_camera()
        {}

        Proxy::~Proxy() {
        }
        
        Container decodePayload(string payload){
            cout << "Decoding payload" <<endl;
            stringstream proto(payload);
            ProtoDeserializerVisitor protoDeserializerVisitor;
            protoDeserializerVisitor.deserializeDataFrom(proto);
            cout << "got the proto payload" <<endl;
            
            //protoDeserializerVisitor.deserializeDataFrom(proto);

            automotive::carolocup::Sensors sd;
            sd.accept(protoDeserializerVisitor);
            cout << "accepted the deserializer" <<endl;
            cout << sd.toString()<< endl;
            int temp = sd.getButtonState();
            bool buttons[4] = {0,0,0,0};
            cout << "temp is" << temp <<endl;
            for (int i = 3; i > 0; i--) buttons[i] = (temp & (1 << i)) != 0;
            Container emptyContainer;
            if(temp < 20){
                cout << "putting data to sbd" <<endl;
              SensorBoardData m_sensorBoardData;
              m_sensorBoardData.putTo_MapOfDistances(0, sd.getUsFront());
              m_sensorBoardData.putTo_MapOfDistances(1, sd.getIrFrontRight());
              m_sensorBoardData.putTo_MapOfDistances(2, sd.getIrRearRight());
              m_sensorBoardData.putTo_MapOfDistances(3, sd.getIrBackRight());
              m_sensorBoardData.putTo_MapOfDistances(4, sd.getUsRear());
              m_sensorBoardData.putTo_MapOfDistances(5, sd.getIrBackLeft());
              m_sensorBoardData.putTo_MapOfDistances(6, sd.getWheelRearLeft());
              m_sensorBoardData.putTo_MapOfDistances(7, sd.getWheelRearRight());
              m_sensorBoardData.putTo_MapOfDistances(8, (int)sd.getGyroHeading());
              m_sensorBoardData.putTo_MapOfDistances(9, buttons[1]);
              m_sensorBoardData.putTo_MapOfDistances(10, buttons[2]);
              m_sensorBoardData.putTo_MapOfDistances(11, buttons[3]);
              m_sensorBoardData.putTo_MapOfDistances(12, sd.getLightReading());
              
              //cout << "wheel encoder left"<< m_sensorBoardData.getValueForKey_MapOfDistances(6) << endl;
              //cout << "wheel encoder right"<< m_sensorBoardData.getValueForKey_MapOfDistances(6) << endl;
              Container sensorData(m_sensorBoardData);
              
              return sensorData;
            }
            cout << "disregarded <<<<<---------------------------------"<< endl;

            return emptyContainer;
        }
        void Proxy::nextString(const string &s) {
         // cout << s << endl; 
         for(int i= 0; i< (int)s.length();i++){
           if(s[i] != flagESC && s[i] != flagEND){
            netstring << s[i];
           }
           if(s[i] == flagESC){
             i++;
             netstring << (char)(s[i]^flagXOR);
           }
           if(s[i] == flagEND){ 
            /*
             for(int j= 0; j< (int)netstring.str().length();j++){
               cout << (int)netstring.str()[j] << ";";
              
             }
             cout << endl; */
            distribute(decodePayload(netstring.str()));
              netstring.str(std::string());
            }
          } 
        }
        void Proxy::setUp() {
            // This method will be call automatically _before_ running body().
            if (getFrequency() < 20) {
                cerr << endl << endl << "Proxy: WARNING! Running proxy with a LOW frequency (consequence: data updates are too seldom and will influence your algorithms in a negative manner!) --> suggestions: --freq=20 or higher! Current frequency: " << getFrequency() << " Hz." << endl << endl << endl;
            }

            // Get configuration data.
            KeyValueConfiguration kv = getKeyValueConfiguration();

            // Create built-in recorder.
            const bool useRecorder = kv.getValue<uint32_t>("proxy.useRecorder") == 1;
            if (useRecorder) {
                // URL for storing containers.
                stringstream recordingURL;
                recordingURL << "file://" << "proxy_" << TimeStamp().getYYYYMMDD_HHMMSS() << ".rec";
                // Size of memory segments.
                const uint32_t MEMORY_SEGMENT_SIZE = getKeyValueConfiguration().getValue<uint32_t>("global.buffer.memorySegmentSize");
                // Number of memory segments.
                const uint32_t NUMBER_OF_SEGMENTS = getKeyValueConfiguration().getValue<uint32_t>("global.buffer.numberOfMemorySegments");
                // Run recorder in asynchronous mode to allow real-time recording in background.
                const bool THREADING = true;
                // Dump shared images and shared data?
                const bool DUMP_SHARED_DATA = getKeyValueConfiguration().getValue<uint32_t>("proxy.recorder.dumpshareddata") == 1;

                m_recorder = unique_ptr<Recorder>(new Recorder(recordingURL.str(), MEMORY_SEGMENT_SIZE, NUMBER_OF_SEGMENTS, THREADING, DUMP_SHARED_DATA));
            }

            // Create the camera grabber.
            const string NAME = getKeyValueConfiguration().getValue<string>("proxy.camera.name");
            string TYPE = getKeyValueConfiguration().getValue<string>("proxy.camera.type");
            std::transform(TYPE.begin(), TYPE.end(), TYPE.begin(), ::tolower);
            const uint32_t ID = getKeyValueConfiguration().getValue<uint32_t>("proxy.camera.id");
            const uint32_t WIDTH = getKeyValueConfiguration().getValue<uint32_t>("proxy.camera.width");
            const uint32_t HEIGHT = getKeyValueConfiguration().getValue<uint32_t>("proxy.camera.height");
            const uint32_t BPP = getKeyValueConfiguration().getValue<uint32_t>("proxy.camera.bpp");
            if (TYPE.compare("opencv") == 0) {
                m_camera = unique_ptr<Camera>(new OpenCVCamera(NAME, ID, WIDTH, HEIGHT, BPP));
            }
            if (TYPE.compare("ueye") == 0) {
#ifdef HAVE_UEYE
                m_camera = unique_ptr<Camera>(new uEyeCamera(NAME, ID, WIDTH, HEIGHT, BPP));
#endif
            }

            if (m_camera.get() == NULL) {
                cerr << "No valid camera type defined." << endl;
            }
            serial->setStringListener(this);
          
            // Start receiving bytes.
            serial->start();
            
        }

        void Proxy::tearDown() {
            // This method will be call automatically _after_ return from body().
            automotive::carolocup::Control cc;
        //cc.setAcceleration(0);
        cc.setSteering(0);
        cc.setLights(0);
        ProtoSerializerVisitor protoSerializerVisitor;
        cc.accept(protoSerializerVisitor);
        stringstream proto;
        protoSerializerVisitor.getSerializedData(proto);
        //stringstream netstring;
        netstring << proto.str().length() << ':' << proto.str() << ',';
        serial->send(netstring.str());
             serial->stop();
        serial->setStringListener(NULL);
        }
        

        void Proxy::distribute(Container c) {
            // Store data to recorder.
            if (m_recorder.get() != NULL) {
                // Time stamp data before storing.
                c.setReceivedTimeStamp(TimeStamp());
                m_recorder->store(c);
            }

            // Share data.
            getConference().send(c);
        }

        // This method will do the main data processing job.
        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode Proxy::body() {
            KeyValueDataStore &kvs = getKeyValueDataStore();
            uint32_t captureCounter = 0;
            while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {
                // Capture frame.
                if (m_camera.get() != NULL) {
                    odcore::data::image::SharedImage si = m_camera->capture();

                    Container c(si);
                    distribute(c);
                    captureCounter++;
                }
                 Container c = kvs.get(VehicleControl::ID());
            VehicleControl vc = c.getData<VehicleControl>();
            
            automotive::carolocup::Control cc;
            cc.setSpeed((int)(vc.getSpeed() * 10));
            cc.setSteering((int)(vc.getSteeringWheelAngle()* (1.0 / (3.141592654 / 180.0))));
            //cout <<"the cc steering is set"<< (int)(30* (1.0 / (3.141592654 / 180.0))) << endl;
            bool lights[3] = {0,0,0}; //stop light, left blinker, right blinker
            lights[0] = vc.getBrakeLights();
            lights[1] = vc.getFlashingLightsLeft();
            lights[2] = vc.getFlashingLightsRight();
            uint lightsINT = 0;
            for(int i = 0; i < 3; i++) if(lights[i]) lightsINT |= 1 << (3 - i);
            cc.setLights(lightsINT);
            cc.setGyroTrigger(1);
            ProtoSerializerVisitor protoSerializerVisitor;
            cc.accept(protoSerializerVisitor);
            stringstream proto;
            protoSerializerVisitor.getSerializedData(proto);
            //cout << cc.toString() << endl;
            stringstream StreamToArduino;

            //char start_flag = 0x12; //18 in DEC
              

            //Whenever a flag or escape byte appears in the message, it is escaped by 0x7D and the byte itself is XOR-ed with 0x20. 
            //So, for example 0x7E becomes 0x7D 0x5E. Similarly 0x7D becomes 0x7D 0x5D. 
            //The receiver unsuffs the escape byte and XORs the next byte with 0x20 again to get the original [6].
            // 8;0;16;0;24;19
            //8;125;93;16;125;51;24;0;19
            for(int i = 0; i < (int)proto.str().length(); i++) {
                if(proto.str()[i] == flagEND || proto.str()[i] == flagESC) {
                    StreamToArduino << flagESC << (char)(proto.str()[i]^flagXOR);
                    i++;
                }
                StreamToArduino << proto.str()[i];
            }
            //cout << StreamToArduino.str() << endl;
            StreamToArduino << flagEND;
            //cout << "length from proxy" << (int)netstring.str().length() << endl;
            if (loopCounter> 2){
                cout << "loopCounter is: "<< loopCounter << endl;
                serial->send(StreamToArduino.str());
            }else loopCounter++;
            cout << "loopCounter is: "<< loopCounter << endl;
            }
            
                // Get sensor data from IR/US.
            

            cout << "Proxy: Captured " << captureCounter << " frames." << endl;

            return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
        }

    }
} // automotive::miniature

