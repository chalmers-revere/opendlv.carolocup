#include "CaroloCupActuators.h"

SteeringMotor::SteeringMotor()
{
	pwm = Adafruit_PWMServoDriver();
	_angle = STRAIGHT_DEGREES;
}

void SteeringMotor::begin(unsigned int min, unsigned int max)
{
	_min_pulse = min;
	_max_pulse = max;
	pwm.begin();

	pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
}

void SteeringMotor::init()
{
	attach(_SERVO_PIN); //attach the servo to its pin
}

int SteeringMotor::convertPulse(int in, int min, int max)
{
	int pulse = map(in, min, max, _min_pulse, _max_pulse);

	return pulse;
}


void SteeringMotor::setAngle(int degrees)
{ // receives some degrees in the scale of MAX_LEFT_ANGLE, MAX_RIGHT_ANGLE

	degrees = constrain(degrees, MAX_LEFT_ANGLE, MAX_RIGHT_ANGLE);
	degrees = convertPulse(degrees, MAX_LEFT_ANGLE, MAX_RIGHT_ANGLE);
	if (degrees != _angle)
	{
		_angle = degrees;
		pwm.setPWM(SERVO_PIN, 0, degrees); //write the appropriate pwm signal to the servo
	}
}

void SteeringMotor::setDegrees(int angle)
{
	angle = constrain(angle, MAX_LEFT_ANGLE,MAX_RIGHT_ANGLE);
	if (angle != _angle)
	{
		_angle = angle;
		write(_angle); //write the appropriate pwm signal to the servo
	}
}

