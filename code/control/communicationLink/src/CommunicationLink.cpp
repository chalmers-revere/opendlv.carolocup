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

#include <stdint.h>

#include <iostream>

#include "odvdcarolocupdatamodel/generated/chalmersrevere/carolocup/ExampleMessage.h"

#include "CommunicationLink.h"

namespace carolocup
{
    namespace control
    {

        using namespace std;
        using namespace odcore::base;

        CommunicationLink::CommunicationLink(const int &argc, char **argv)
                : TimeTriggeredConferenceClientModule(argc, argv, "carolocup-communicationlink")
        {}

        CommunicationLink::~CommunicationLink()
        {}

        void CommunicationLink::setUp()
        {}

        void CommunicationLink::tearDown()
        {}

        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode CommunicationLink::body()
        {

            while (getModuleStateAndWaitForRemainingTimeInTimeslice() ==
                   odcore::data::dmcp::ModuleStateMessage::RUNNING)
            {
                cout << "Inside the main processing loop." << endl;
            }

            return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
        }
    }
} // carolocup::control
