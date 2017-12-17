/**
 * PID - PID code.
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

#ifndef PID_H
#define PID_H

#include "../../defines/defines.h"

#include <iostream>
#include <stdint.h>
#include <memory>
#include <math.h>
#include <cstdio>
#include <cmath>

namespace carolocup
{
	namespace control
	{

		using namespace std;

		class PID
		{
		public:
			/**
			 * Constructor
			 */
			PID();

			/**
			 * Destructor.
			 */
			virtual ~PID();

			/**
			 * Initialize PID.
			 * @param kp the proportional value for PID controller
			 * @param ki the integral value for PID controller
			 * @param kd the derivative value for PID controller
			 */
			void init(double kp, double ki, double kd);

			/**
			 * Update the PID error variables given cross track error
			 * @param cte the current cross track error
			 */
			void updateError(double cte);

			/**
			 * Compute the control command value according to PID controller
			 * @return the steering angle
			 */
			double outputSteerAng();

			/**
			 * Compute the throttle by PID controller
			 * @param max_thro max throttle value
			 * @return the computed throttle value
			 */
			double outputThrottle(double max_thro);

			double getKP();

			double getKI();

			double getKD();

		private:
			/**
			 * Error terms for PID controller
			 */
			double p_error;
			double i_error;
			double d_error;

			/**
			 * Coefficients
			 */
			double Kp;
			double Ki;
			double Kd;
		};
	}
} // carolocup::control

#endif /* PID_H */
