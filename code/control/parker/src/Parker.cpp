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

		Parker::Parker(const int &argc, char **argv)
				: DataTriggeredConferenceClientModule(argc, argv, "carolocup-parker")
		{}

		Parker::~Parker()
		{}

		void Parker::setUp()
		{}

		void Parker::tearDown()
		{}

		void Parker::nextContainer(Container &c)
		{
			if (c.getDataType() == CommunicationLinkMSG::ID())
			{
				Container communicationLinkContainer = c.getData<CommunicationLinkMSG>();
				const CommunicationLinkMSG communicationLinkMSG = communicationLinkContainer.getData<CommunicationLinkMSG>();

			}
		}

	}
} // carolocup::control
