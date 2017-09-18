/**
 * Example - Example code.
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

#ifndef CONTROL_LIDARHANDLER_H
#define CONTROL_LIDARHANDLER_H

#include <opendavinci/odcore/base/module/TimeTriggeredConferenceClientModule.h>
#include "sweep/sweep.hpp"
#include <cmath>
#include <iostream>
#include <memory>
#include <stdint.h>
#include <string>
#include <vector>
#include <cctype>
#include <algorithm>
#include <stdlib.h>

#define PORT "/dev/ttyUSB0"
#define MAX_DIS 301
#define MIN_DIS 2

namespace carolocup
{
    namespace control
    {

        using namespace std;
        using namespace sweep;

/**
 * Time-triggered LIDARHandler.
 */
        class LIDARHandler : public odcore::base::module::TimeTriggeredConferenceClientModule
        {
        private:
            LIDARHandler(const LIDARHandler & /*obj*/) = delete;

            LIDARHandler &operator=(const LIDARHandler & /*obj*/) = delete;

        public:
            /**
             * Constructor.
             *
             * @param argc Number of command line arguments.
             * @param argv Command line arguments.
             */
            LIDARHandler(const int &argc, char **argv);

            virtual ~LIDARHandler();

        private:
            void setUp();

            void tearDown();

            odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode body();

            void startDevice();

            void stopDevice();

            sweep::sweep device;
            map <uint32_t, vector<int>> lidar;
            map <uint32_t, int> lidar_data;
            int count;
        };
    }
} // carolocup::control

#endif /*CONTROL_LIDARHandler_H*/
