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

	angle = constrain(angle, MAX_LEFT_ANGLE,MAX_RIGHT_ANGLE);
	if (angle != _angle)
	{
		_angle = angle;
		write(_angle); //write the appropriate pwm signal to the servo
	}
}


