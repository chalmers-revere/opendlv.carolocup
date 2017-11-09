#include "CaroloCupActuators.h"

SteeringMotor::SteeringMotor()
{
	_angle = STRAIGHT_DEGREES;
}

void SteeringMotor::init()
{
	attach(_SERVO_PIN); //attach the servo to its pin
}

void SteeringMotor::setAngle(int degrees)
{ // receives some degrees in the scale of MAX_LEFT_ANGLE, MAX_RIGHT_ANGLE

	degrees = constrain(degrees, MAX_LEFT_ANGLE,MAX_RIGHT_ANGLE);
	if (degrees != _angle)
	{
		_angle = degrees;
		write(_angle); //write the appropriate pwm signal to the servo
	}
}


