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

#include "PID.h"

namespace carolocup
{
	namespace control
	{

		using namespace std;

		/**
 * Constructor
 */
		PID::PID():
		p_error(0),
		i_error(0),
		d_error(0),
		Kp(0),
		Ki(0),
		Kd(0)
		{}

/**
 * Destructor.
 */
		PID::~PID()
		{}


/**
 * Initialize PID.
 * @param kp the proportional value for PID controller
 * @param ki the integral value for PID controller
 * @param kd the derivative value for PID controller
 */
		void PID::init(double kp, double ki, double kd)
		{
			Kp = kp;
			Ki = ki;
			Kd = kd;

			p_error = 0;
			i_error = 0;
			d_error = 0;
		}


/**
 * Update the PID error variables given cross track error
 * @param cte the current cross track error
 */
		void PID::updateError(double cte)
		{

			double pre_cte = p_error;

			p_error = cte;
			i_error += cte;
			d_error = cte - pre_cte;
		}


/**
 * Compute the control command value according to PID controller
 * @return the steering angle
 */
		double PID::outputSteerAng()
		{

			return -Kp * p_error - Ki * i_error - Kd * d_error;
		}


/**
 * Compute the throttle by PID controller
 * @param max_thro max throttle value
 * @return the computed throttle value
 */
		double PID::outputThrottle(double max_thro)
		{

			return max_thro - Kp * p_error - Ki * i_error - Kd * d_error;
		}


		double PID::getKP()
		{
			return Kp;
		}

		double PID::getKI()
		{
			return Ki;
		}

		double PID::getKD()
		{
			return Kd;
		}

	}
} // carolocup::control
