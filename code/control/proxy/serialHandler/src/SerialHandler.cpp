/**
 * SerialHandler - SerialHandler code.
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

#include <stdint.h>

#include <iostream>

#include "odvdcarolocupdatamodel/generated/chalmersrevere/carolocup/ExampleMessage.h"

#include "SerialHandler.h"

namespace carolocup
{
	namespace control
	{

		using namespace std;
		using namespace odcore::base;

		SerialHandler::SerialHandler(const int &argc, char **argv)
				: TimeTriggeredConferenceClientModule(argc, argv, "carolocup-serialhandler")
		{
			for (int i = 0; i < argc; ++i)  //TODO receive command line argument to define serial behaviour
			{
				cout << argv[i] << endl; //the argument we are looking for is the i=3
			}
		}

		SerialHandler::~SerialHandler()
		{}

		void SerialHandler::setUp()
		{}

		void SerialHandler::tearDown()
		{}

		odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode SerialHandler::body()
		{

			while (getModuleStateAndWaitForRemainingTimeInTimeslice() ==
				   odcore::data::dmcp::ModuleStateMessage::RUNNING)
			{
				//cout << "Inside the main processing loop." << endl;
			}

			return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
		}
	}
} // carolocup::control
