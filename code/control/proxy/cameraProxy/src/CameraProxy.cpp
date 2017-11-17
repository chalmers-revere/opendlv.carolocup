/**
 * CameraProxy - CameraProxy code.
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

#include "CameraProxy.h"

namespace carolocup
{
	namespace control
	{
		using namespace std;
		using namespace odcore::base;
		using namespace odcore::data;
		using namespace odtools::recorder;

		CameraProxy::CameraProxy(const int &argc, char **argv)
				: TimeTriggeredConferenceClientModule(argc, argv, "carolocup-cameraproxy"),
				  m_recorder(),
				  m_camera()
		{}

		CameraProxy::~CameraProxy()
		{}

		void CameraProxy::setUp()
		{
			// This method will be call automatically _before_ running body().
			if (getFrequency() < 20)
			{
				cerr << endl << endl
					 << "CameraProxy: WARNING! Running cameraProxy with a LOW frequency (consequence: data updates are too seldom and will influence your algorithms in a negative manner!) --> suggestions: --freq=20 or higher! Current frequency: "
					 << getFrequency() << " Hz." << endl << endl << endl;
			}

			// Get configuration data.
			KeyValueConfiguration kv = getKeyValueConfiguration();

			//cerr << kv.toString() << endl;

//			const vector <string> vkv = kv.getListOfKeys();
//			for (unsigned int i = 0; i < vkv.size(); i++)
//			{
//				cout << "value -> " << vkv[i] << endl;  // prints d.
//
//			}

			// Create built-in recorder.
			const bool useRecorder = kv.getValue<uint32_t>("global.cameraproxy.useRecorder") == 1;
			if (useRecorder)
			{
				// URL for storing containers.
				stringstream recordingURL;

				recordingURL << "file://" << "proxy_" << TimeStamp().getYYYYMMDD_HHMMSS_noBlankNoColons() << ".rec";

				// Size of memory segments.
				const uint32_t MEMORY_SEGMENT_SIZE = kv.getValue<uint32_t>("global.buffer.memorySegmentSize");
				// Number of memory segments.
				const uint32_t NUMBER_OF_SEGMENTS = kv.getValue<uint32_t>("global.buffer.numberOfMemorySegments");
				// Run recorder in asynchronous mode to allow real-time recording in background.
				const bool THREADING = true;
				// Dump shared images and shared data?
				const bool DUMP_SHARED_DATA = kv.getValue<uint32_t>("global.cameraproxy.camera.dumpSharedData") == 1;

				m_recorder = unique_ptr<Recorder>(
						new Recorder(recordingURL.str(), MEMORY_SEGMENT_SIZE, NUMBER_OF_SEGMENTS, THREADING,
									 DUMP_SHARED_DATA));
			}

			// Create the camera grabber.
			const string NAME = kv.getValue<string>("global.cameraproxy.camera.name");
			string TYPE = kv.getValue<string>("global.cameraproxy.camera.type");
			transform(TYPE.begin(), TYPE.end(), TYPE.begin(), ::tolower);
			const uint32_t ID = kv.getValue<uint32_t>("global.cameraproxy.camera.id");
			const uint32_t WIDTH = kv.getValue<uint32_t>("global.cameraproxy.camera.width");
			const uint32_t HEIGHT = kv.getValue<uint32_t>("global.cameraproxy.camera.height");
			const uint32_t BPP = kv.getValue<uint32_t>("global.cameraproxy.camera.bpp");

			if (TYPE.compare("opencv") == 0)
			{
				m_camera = unique_ptr<Camera>(new OpenCVCamera(NAME, ID, WIDTH, HEIGHT, BPP));
			}
			else
			{
				cerr << "Use \"openCV\" as camera type. " << endl;
			}

			if (m_camera.get() == NULL)
			{
				cerr << "No valid camera type defined." << endl;
			}
		}

		void CameraProxy::tearDown()
		{
			// This method will be call automatically _after_ return from body().
			cerr << "CameraProxy stopped." << endl;
		}

		odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode CameraProxy::body()
		{
			uint32_t captureCounter = 0;

			while (getModuleStateAndWaitForRemainingTimeInTimeslice() ==
				   odcore::data::dmcp::ModuleStateMessage::RUNNING)
			{
				// Capture frame.
				if (m_camera.get() != NULL)
				{
					image::SharedImage si = m_camera->capture();

					Container c(si);
					distribute(c);
					captureCounter++;
				}
			}

			cout << "CameraProxy: Captured " << captureCounter << " frames." << endl;

			return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
		}

		void CameraProxy::distribute(Container c)
		{
			// Store data to recorder.
			if (m_recorder.get())
			{
				c.setReceivedTimeStamp(TimeStamp());
				m_recorder->store(c);
				//cerr << "m_record " << endl;
			}

			// Share container with image
			getConference().send(c);
		}
	}
} // carolocup::control
