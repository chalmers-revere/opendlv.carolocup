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
	pinMode(HEADLIGHTS, OUTPUT);
}

void LEDControl::setHeadLights(unsigned int state)
{
	state = constrain(state, 0, 1);
	digitalWrite(HEADLIGHTS, state);
}

void LEDControl::setBrakeLights(unsigned int value)
{
	if (value == 2) //flash light
	{
		for (int i = 0; i < 2000; ++i)
		{
			digitalWrite(BRAKE_LIGHTS, HIGH);
		}

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

void LEDControl::setIndicators(unsigned int state, unsigned int frequency)
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

				wait(frequency);

				digitalWrite(INDICATOR_BACK_RIGHT, LOW);
				digitalWrite(INDICATOR_FRONT_RIGHT, LOW);
				digitalWrite(INDICATOR_BACK_LEFT, LOW);
				digitalWrite(INDICATOR_FRONT_LEFT, LOW);

				wait(frequency);
			}
			break;
		case INDICATOR_FRONT_LEFT:
			digitalWrite(INDICATOR_FRONT_LEFT, HIGH);
			wait(frequency);
			digitalWrite(INDICATOR_FRONT_LEFT, LOW);
			break;
		case INDICATOR_FRONT_RIGHT:
			digitalWrite(INDICATOR_FRONT_RIGHT, HIGH);
			wait(frequency);
			digitalWrite(INDICATOR_FRONT_RIGHT, LOW);
			break;
		case INDICATOR_BACK_LEFT:
			digitalWrite(INDICATOR_BACK_LEFT, HIGH);
			wait(frequency);
			digitalWrite(INDICATOR_BACK_LEFT, LOW);
			break;
		case INDICATOR_BACK_RIGHT:
			digitalWrite(INDICATOR_BACK_RIGHT, HIGH);
			wait(frequency);
			digitalWrite(INDICATOR_BACK_RIGHT, LOW);
			break;
		default:
			break;
	}
}

void LEDControl::wait(long milliseconds)
{
	interval = milliseconds;
	currentMillis = millis();
	while (millis() - currentMillis <= interval);
}