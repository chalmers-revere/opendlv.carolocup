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

void LEDControl::setBrakeLights()
{
	digitalWrite(BRAKE_LIGHTS, HIGH);
	wait(300);
	digitalWrite(BRAKE_LIGHTS, LOW);
}

void LEDControl::setRCLight()
{
	digitalWrite(RC_LIGHT, HIGH);
	wait(300);
	digitalWrite(RC_LIGHT, LOW);
}

void LEDControl::setIndicators(unsigned int id, unsigned int state)
{
	switch (state)
	{ //LEDS has 4 states; 0 off, 1 on, 2 blinking, 3 blink 3 times
		case 0:
			switch (id) {
				case INDICATOR_FRONT_LEFT:
					digitalWrite(INDICATOR_FRONT_LEFT, HIGH);
					wait(500);
					digitalWrite(INDICATOR_FRONT_LEFT, LOW);
					break;
				case INDICATOR_FRONT_RIGHT:
					digitalWrite(INDICATOR_FRONT_RIGHT, HIGH);
					wait(500);
					digitalWrite(INDICATOR_FRONT_RIGHT, LOW);
					break;
				case INDICATOR_BACK_LEFT:
					digitalWrite(INDICATOR_BACK_LEFT, HIGH);
					wait(500);
					digitalWrite(INDICATOR_BACK_LEFT, LOW);
					break;
				case INDICATOR_BACK_RIGHT:
					digitalWrite(INDICATOR_BACK_RIGHT, HIGH);
					wait(500);
					digitalWrite(INDICATOR_BACK_RIGHT, LOW);
					break;
			    default:
			        break;
			 }
			break;
		case 1:
			for (int i = 0; i < 3; ++i)
			{
				digitalWrite(INDICATOR_BACK_RIGHT, HIGH);
				digitalWrite(INDICATOR_FRONT_RIGHT, HIGH);
				digitalWrite(INDICATOR_BACK_LEFT, HIGH);
				digitalWrite(INDICATOR_FRONT_LEFT, HIGH);

				wait(500);

				digitalWrite(INDICATOR_BACK_RIGHT, LOW);
				digitalWrite(INDICATOR_FRONT_RIGHT, LOW);
				digitalWrite(INDICATOR_BACK_LEFT, LOW);
				digitalWrite(INDICATOR_FRONT_LEFT, LOW);
			}
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