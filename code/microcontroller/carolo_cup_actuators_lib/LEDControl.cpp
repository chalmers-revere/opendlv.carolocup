#include "CaroloCupActuators.h"

LEDControl::LEDControl()
{}

void LEDControl::begin()
{
	pinMode(BRAKE_LIGHTS, OUTPUT);
	pinMode(RC_LIGHT, OUTPUT);
	pinMode(INDICATOR_BACK_LEFT, OUTPUT);
	pinMode(INDICATOR_BACK_RIGHT, OUTPUT);
	pinMode(INDICATOR_FRONT_LEFT, OUTPUT);
	pinMode(INDICATOR_FRONT_RIGHT, OUTPUT);
	pinMode(STATUS_LIGHT, OUTPUT);
}

void LEDControl::setStatusLight(unsigned int state)
{
	state = constrain(state, 0, 1);
	digitalWrite(STATUS_LIGHT, state);
}

void LEDControl::setBrakeLights(unsigned int value)
{
	if (value == 2) //blink
	{
		digitalWrite(BRAKE_LIGHTS, HIGH);

		wait(0.5);

		digitalWrite(BRAKE_LIGHTS, LOW);
	}
	else
	{
		value = constrain(value, LOW, HIGH);
		digitalWrite(BRAKE_LIGHTS, value);
	}
}

void LEDControl::setRCLight(unsigned int frequency, unsigned long counter)
{
	if (frequency)
	{
		if (counter % frequency == 0) digitalWrite(RC_LIGHT, HIGH);
		else digitalWrite(RC_LIGHT, LOW);
	}
	else
	{
		digitalWrite(RC_LIGHT, LOW);
	}
}

void LEDControl::setIndicators(unsigned int state, double seconds)
{
	switch (state)
	{
		case ID_OUT_LIGHTS_EFFECT:
			for (int i = 0; i < 3; ++i)
			{
				digitalWrite(INDICATOR_BACK_RIGHT, HIGH);
				digitalWrite(INDICATOR_FRONT_RIGHT, HIGH);
				digitalWrite(INDICATOR_BACK_LEFT, HIGH);
				digitalWrite(INDICATOR_FRONT_LEFT, HIGH);

				wait(seconds);

				digitalWrite(INDICATOR_BACK_RIGHT, LOW);
				digitalWrite(INDICATOR_FRONT_RIGHT, LOW);
				digitalWrite(INDICATOR_BACK_LEFT, LOW);
				digitalWrite(INDICATOR_FRONT_LEFT, LOW);

				wait(seconds);
			}
			break;
		case LED_SIGNAL:
			for (int j = 0; j < 2; ++j)
			{
				digitalWrite(INDICATOR_BACK_RIGHT, HIGH);
				digitalWrite(INDICATOR_FRONT_RIGHT, HIGH);
				digitalWrite(INDICATOR_BACK_LEFT, HIGH);
				digitalWrite(INDICATOR_FRONT_LEFT, HIGH);
				digitalWrite(RC_LIGHT, HIGH);
				digitalWrite(BRAKE_LIGHTS, HIGH);
				digitalWrite(STATUS_LIGHT, HIGH);

				wait(seconds);

				digitalWrite(INDICATOR_BACK_RIGHT, LOW);
				digitalWrite(INDICATOR_FRONT_RIGHT, LOW);
				digitalWrite(INDICATOR_BACK_LEFT, LOW);
				digitalWrite(INDICATOR_FRONT_LEFT, LOW);
				digitalWrite(RC_LIGHT, LOW);
				digitalWrite(BRAKE_LIGHTS, LOW);
				digitalWrite(STATUS_LIGHT, LOW);

				wait(seconds);
			}
			break;
		case INDICATOR_FRONT_LEFT:
			digitalWrite(INDICATOR_FRONT_LEFT, HIGH);
			wait(seconds);
			digitalWrite(INDICATOR_FRONT_LEFT, LOW);
			break;
		case INDICATOR_FRONT_RIGHT:
			digitalWrite(INDICATOR_FRONT_RIGHT, HIGH);
			wait(seconds);
			digitalWrite(INDICATOR_FRONT_RIGHT, LOW);
			break;
		case INDICATOR_BACK_LEFT:
			digitalWrite(INDICATOR_BACK_LEFT, HIGH);
			wait(seconds);
			digitalWrite(INDICATOR_BACK_LEFT, LOW);
			break;
		case INDICATOR_BACK_RIGHT:
			digitalWrite(INDICATOR_BACK_RIGHT, HIGH);
			wait(seconds);
			digitalWrite(INDICATOR_BACK_RIGHT, LOW);
			break;
		default:
			break;
	}
}

void LEDControl::wait(double seconds)
{
	interval = seconds * 1000000;
	currentMillis = micros();
	while (micros() - currentMillis <= interval);
}