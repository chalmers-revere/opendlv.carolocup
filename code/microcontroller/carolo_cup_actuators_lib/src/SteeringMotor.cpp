#include "CaroloCupActuators.h"

SteeringMotor::SteeringMotor()
{
	pwm = Adafruit_PWMServoDriver();
	_angle = 90;
}

void SteeringMotor::begin()
{
	pwm.begin();

	pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
}

int SteeringMotor::convertPulse(int in, int min, int max)
{
	int pulse = map(angle, min, max, SERVOMIN, SERVOMAX);
}



void SteeringMotor::setAngle(int degrees)
{ // receives some degrees in the scale of MAX_LEFT_ANGLE, MAX_RIGHT_ANGLE
	degrees = constrain(degrees, MAX_LEFT_ANGLE, MAX_RIGHT_ANGLE);
	degrees = convertPulse(degrees, MAX_LEFT_ANGLE, MAX_RIGHT_ANGLE);
	pwm.setPWM(SERVO_PIN, 0, degrees);
}

