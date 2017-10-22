/**
 * Overtaker - Overtaker code.
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

#include "Overtaker.h"

namespace carolocup
{
    namespace control
    {

        using namespace std;
        using namespace odcore::base;
        using namespace odcore::data;
        using namespace odcore::data::image;
        using namespace odcore::wrapper;
        using namespace odcore::data::dmcp;
        using namespace gap;

        Overtaker::Overtaker(const int &argc, char **argv)
                : DataTriggeredConferenceClientModule(argc, argv, "carolocup-overtaker"),
                  m_vehicleControl(),
                  Sim(false),
                  _state(0),
                  overtakerMSG(),
                  OVERTAKING_DISTANCE(48.0),
                  HEADING_PARALLEL(10),
                  SPEED_CAR(96),
                  TURN_ANGLE_CAR_LEFT(-1.5),
                  TURN_ANGLE_CAR_RIGHT(1.5),
                  STRAIGHT_ANGLE_CAR(0),
                  IR_RR(0),
                  IR_FR(0),
                  IR_BACK(0),
                  US_C(0),
                  US_R(0),
                  odometerReal(0),
                  oldOdometer(0),
                  odo(0),
                  us_c_old(0),
                  _us_c_old(0),
                  stage(FIND_OBJECT),
                  distanceToObstacle(0),
                  distanceToObstacleOld(0),
                  OBJECT_PLAUSIBLE_COUNT(3),
                  objectPlausibleCount(0),
                  objectDangerCount(0),
                  _stop(0),
                  valid_us(0),
                  park_state(0),
                  flag(0)
        {}

        Overtaker::~Overtaker()
        {}

        void Overtaker::setUp()
        {
            KeyValueConfiguration kv = getKeyValueConfiguration();
            Sim = kv.getValue<int32_t>("global.sim") == 1;
        }

        void Overtaker::tearDown()
        {}

        void Overtaker::nextContainer(Container &c)
        {

            if (c.getDataType() == CommunicationLinkMSG::ID())
            {
                Container communicationLinkContainer = c.getData<CommunicationLinkMSG>();
                const CommunicationLinkMSG communicationLinkMSG = communicationLinkContainer.getData<CommunicationLinkMSG>();

            }
        }

        void Overtaker::measuringMachine()
        {

            if (stage == FIND_OBJECT)
            {


            } else if (stage == FIND_OBJECT_PLAUSIBLE)
            {

            } else if (stage == HAVE_BOTH_IR)
            {


            } else if (stage == KEEP_TURN_RIGHT)
            {


            } else if (stage == HAVE_NO_READING)
            {

            } else if (stage == KEEP_TURN_RIGHT_END)
            {

            } else if (stage == ADJUST_TO_STRAIGHT)
            {

            } else if (stage == DISABLE)
            {

            }

            Container container(m_vehicleControl);
            getConference().send(container);
        }
    }
} // carolocup::control
