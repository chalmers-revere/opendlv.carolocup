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
		using namespace odcore::data;

		/**
 * Constructor
 */
		PID::PID():
		p_error(0),
		i_error(0),
		d_error(0),
		Kp(0),
		Ki(0),
		Kd(0),
		e(0),
		m_previousTime(),
		m_eSum(0),
		m_eOld(0)
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
		void PID::init(double _kp, double _ki, double _kd)
		{
			Kp = _kp;
			Ki = _ki;
			Kd = _kd;

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
			e = cte;

			TimeStamp currentTime;
			double timeStep = (currentTime.toMicroseconds() - m_previousTime.toMicroseconds()) / (1000.0 * 1000.0);
			m_previousTime = currentTime;

			// A way to handle toggling in corners
			if (fabs(e) < 1e-2)
			{
				m_eSum = 0;
			}
			else
			{
				m_eSum += e;
			}

			// PID control algorithm uses the following values, with the meaning:
			//Kp = p_gain -> Proportional -> the oscillation in the trajectory as the algorithm attempts to approach the error state to 0, moving towards the goal
			//Ki = i_gain-> Integral -> Current -> how long/ how much of the oscillation is spent on either side of the goal, "look at the past"
			//Kd = d_gain-> derivative -> how sharp the approximation to the goal is made, directly influencing the oscillation
			p_error = Kp * e;
			i_error = Ki * timeStep * m_eSum;
			d_error = Kd * (e - m_eOld) / timeStep;

			cerr << currentTime.getYYYYMMDD_HHMMSS_noBlank() << " ERROR -> " << e << endl;
			m_eOld = e;
		}

/**
 * Update the PID gain variables
 * @param kp the proportional value for PID controller
 * @param ki the integral value for PID controller
 * @param kd the derivative value for PID controller
 */
		void PID::updateGains(double _kp, double _ki, double _kd)
		{
			Kp = _kp;
			Ki = _ki;
			Kd = _kd;
		}

/**
 * Compute the control command value according to PID controller
 * @return the steering angle
 */
		double PID::outputSteerAng()
		{
			const double y = p_error + i_error + d_error;

			double desiredSteering = 0;

			if (fabs(e) > 1e-2)
			{
				desiredSteering = y;
			}
			// Set an upper and lower limit for the desired steering
			if (desiredSteering > 1.5)
			{
				desiredSteering = 1.5;
			}
			if (desiredSteering < -1.5)
			{
				desiredSteering = -1.5;
			}

			return desiredSteering;
		}


/**
 * Compute the throttle by PID controller
 * @param max_thro max throttle value
 * @return the computed throttle value
 */
		int PID::outputThrottle(double max_thro)
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
