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

#ifndef CONTROL_CAMERAPROXY_H
#define CONTROL_CAMERAPROXY_H

#include "../../../defines/defines.h"

#include <map>
#include <memory>
#include <iostream>
#include <stdint.h>

#include "opendavinci/odcore/base/KeyValueConfiguration.h"
#include "opendavinci/odcore/data/TimeStamp.h"
#include "OpenCVCamera.h"
#include "opendavinci/odcore/base/module/TimeTriggeredConferenceClientModule.h"
#include "opendavinci/odcore/data/Container.h"
#include "opendavinci/odtools/recorder/Recorder.h"

#include "Camera.h"

#ifdef HAVE_UEYE
#include "uEyeCamera.h"
#endif

namespace carolocup
{
	namespace control
	{

		using namespace std;

/**
 * Time-triggered CameraProxy.
 */
		class CameraProxy : public odcore::base::module::TimeTriggeredConferenceClientModule
		{
		private:
			CameraProxy(const CameraProxy & /*obj*/) = delete;

			CameraProxy &operator=(const CameraProxy & /*obj*/) = delete;

		public:
			/**
			 * Constructor.
			 *
			 * @param argc Number of command line arguments.
			 * @param argv Command line arguments.
			 */
			CameraProxy(const int &argc, char **argv);

			virtual ~CameraProxy();

		private:
			void setUp();

			void tearDown();

			odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode body();

			void distribute(odcore::data::Container c);

			unique_ptr <odtools::recorder::Recorder> m_recorder;

			unique_ptr <Camera> m_camera;
		};
	}
} // carolocup::control

#endif /*CONTROL_CAMERAPROXY_H*/
