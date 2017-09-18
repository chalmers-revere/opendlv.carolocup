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

#include <stdint.h>

#include <iostream>

#include "odvdcarolocupdatamodel/generated/chalmersrevere/carolocup/ExampleMessage.h"

#include "LIDARHandler.h"

namespace carolocup
{
    namespace control
    {

        using namespace std;
        using namespace odcore::base;

        LIDARHandler::LIDARHandler(const int &argc, char **argv)
                : TimeTriggeredConferenceClientModule(argc, argv, "carolocup-lidarhandler"),
                  device{PORT},
                  lidar(),
                  lidar_data(),
                  count(0)
        {}

        LIDARHandler::~LIDARHandler()
        {}

        void LIDARHandler::setUp()
        {
            startDevice();
        }

        void LIDARHandler::tearDown()
        {
            stopDevice();
        }

        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode LIDARHandler::body()
        {
            while (getModuleStateAndWaitForRemainingTimeInTimeslice() ==
                   odcore::data::dmcp::ModuleStateMessage::RUNNING)
            {
                cout << "Inside the main processing loop!." << endl;
                try
                {
                    if (count == 3)
                    {
                        for (int i = 0; i <= 90; ++i)
                        {
                            if (lidar[i].size() > 0)
                            {
                                sort(lidar[i].begin(), lidar[i].end());
                                int med = (int) lidar[i].size() / 2;
                                lidar_data[i] = lidar[i][med];
                            }
                            lidar[i].clear();
                            std::cout << "angle-"
                                      << i
                                      << " distance ->" << lidar_data[i] << "\n";
                        }
                        for (int i = 270; i <= 360; ++i)
                        {
                            if (lidar[i].size() > 0)
                            {
                                sort(lidar[i].begin(), lidar[i].end());
                                int med = (int) lidar[i].size() / 2;
                                lidar_data[i] = lidar[i][med];
                            }
                            lidar[i].clear();
                            std::cout << "angle-"
                                      << i
                                      << " distance ->" << lidar_data[i] << "\n";
                        }
                        count = 0;
                    }

                    const sweep::scan scan = device.get_scan();

                    for (const sweep::sample &sample : scan.samples)
                    {
//                        std::cout << "angle "
//                                  << (int) std::fmod((static_cast<double>(sample.angle) / 1000. + 90.), 360.)
//                                  << " distance " << sample.distance << " strength "
//                                  << sample.signal_strength << "\n";

                        int angle = (sample.angle / 1000) % 360;
                        if (angle < 91 || angle > 269)
                        {
                            int distance = sample.distance;
                            if (distance > MIN_DIS && distance < MAX_DIS)
                            {
                                lidar[angle].push_back(distance);
                            } else
                            {
                                lidar[angle].push_back(-1);
                            }
                        }
                    }
                    count++;
                } catch (const sweep::device_error &e)
                {
                    std::cerr << "Error: " << e.what() << std::endl;
                    exit(EXIT_FAILURE);
                }
            }

            return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
        }

        void LIDARHandler::startDevice()
        {
            try
            {
                cout << "Constructing sweep device..." << endl;
                device.set_motor_speed(1);
                device.set_sample_rate(500);
                device.set_motor_speed(10);
                cout << "Motor Speed Setting: " << device.get_motor_speed() << " Hz" << endl;
                cout << "Sample Rate Setting: " << device.get_sample_rate() << " Hz" << endl;

                cout << "Beginning data acquisition as soon as motor speed stabilizes..." << endl;
                device.start_scanning();
            } catch (const sweep::device_error &e)
            {
                cerr << "Error: " << e.what() << endl;
                exit(EXIT_FAILURE);
            }
        }

        void LIDARHandler::stopDevice()
        {
            try
            {
                device.stop_scanning();
            } catch (const sweep::device_error &e)
            {
                cerr << "Error: " << e.what() << endl;
            }
        }
    }
} // carolocup::control
