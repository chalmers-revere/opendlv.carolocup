/**
 * SerialOutHandler - SerialOutHandler code.
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

#ifndef CONTROL_SERIALOUTHANDLER_H
#define CONTROL_SERIALOUTHANDLER_H

#include "../../../defines/defines.h"

#include <opendavinci/odcore/base/module/TimeTriggeredConferenceClientModule.h>

namespace carolocup
{
    namespace control
    {

        using namespace std;

/**
 * Time-triggered SerialOutHandler.
 */
        class SerialOutHandler : public odcore::base::module::TimeTriggeredConferenceClientModule
        {
        private:
            SerialOutHandler(const SerialOutHandler & /*obj*/) = delete;

            SerialOutHandler &operator=(const SerialOutHandler & /*obj*/) = delete;

        public:
            /**
             * Constructor.
             *
             * @param argc Number of command line arguments.
             * @param argv Command line arguments.
             */
            SerialOutHandler(const int &argc, char **argv);

            virtual ~SerialOutHandler();

        private:
            void setUp();

            void tearDown();

            odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode body();
        };
    }
} // carolocup::control

#endif /*CONTROL_SERIALHANDLER_H*/
