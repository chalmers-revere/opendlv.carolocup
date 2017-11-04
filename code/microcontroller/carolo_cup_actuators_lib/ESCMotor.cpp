#include "CaroloCupActuators.h"

ESCMotor::ESCMotor()
{
	_speed = IDLE_SPEED;
}

void ESCMotor::init()
{
	_pin = ESC_PIN;
	attach(_pin); //attach the servo to its pin
	arm();
}

void ESCMotor::arm()
{
	write(IDLE_SPEED);
}

void ESCMotor::setSpeed(int speed)
{ //receives a speed in the scale of 0 to 180
	speed = constrain(speed, FULL_BACKWARD, FULL_FORWARD);
	if (speed != _speed)
	{
		_speed = speed;
		write(_speed); //write the appropriate pwm signal to the servo
	}
}

void ESCMotor::brake()
{
	if (_speed < IDLE_SPEED)
	{
		write(IDLE_SPEED + 15);
	}
	else if (_speed > IDLE_SPEED)
	{
		write(IDLE_SPEED - 55);
	}
	write(IDLE_SPEED);
	_speed = IDLE_SPEED;
}
