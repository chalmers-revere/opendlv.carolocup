#include "CaroloCupActuators.h"

LED::LED()
{}

void LED::begin()
{
	pinMode(BRAKE_LIGHTS, OUTPUT);
	pinMode(RC_LIGHT, OUTPUT);
	pinMode(INDICATOR_BACK_LEFT, OUTPUT);
	pinMode(INDICATOR_BACK_RIGHT, OUTPUT);
	pinMode(INDICATOR_FRONT_LEFT, OUTPUT);
	pinMode(INDICATOR_FRONT_RIGHT, OUTPUT);
	pinMode(HEADLIGHTS, OUTPUT);
}

void LED::setLED(int pin, int state)
{
	state = constrain(state, LOW, HIGH);
	digitalWrite(pin, state);
}