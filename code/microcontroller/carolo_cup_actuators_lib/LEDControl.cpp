#include "CaroloCupActuators.h"

LEDControl::LEDControl() {}

void LEDControl::begin() {
    pinMode(BRAKE_LIGHTS, OUTPUT);
    pinMode(RC_LIGHT, OUTPUT);
    pinMode(INDICATOR_BACK_LEFT, OUTPUT);
    pinMode(INDICATOR_BACK_RIGHT, OUTPUT);
    pinMode(INDICATOR_FRONT_LEFT, OUTPUT);
    pinMode(INDICATOR_FRONT_RIGHT, OUTPUT);
    pinMode(HEADLIGHTS, OUTPUT);
}

void LEDControl::setHeadLights(unsigned int state) {
    state = constrain(state, 0, 1);
    digitalWrite(HEADLIGHTS, state);
}

void LEDControl::setBrakeLights(unsigned int value) {
    if (value == 2) //flash light
    {
        for (int i = 0; i < 2000; ++i) {
            digitalWrite(BRAKE_LIGHTS, HIGH);
        }

        digitalWrite(BRAKE_LIGHTS, LOW);
    } else {
        value = constrain(value, LOW, HIGH);
        digitalWrite(BRAKE_LIGHTS, value);
    }
}

void LEDControl::setRCLight(double wait) {
    digitalWrite(RC_LIGHT, HIGH);
    wait(wait);
    digitalWrite(RC_LIGHT, LOW);
}

void LEDControl::setIndicators(unsigned int state, double seconds) {
    switch (state) {
        case ID_OUT_LIGHTS_EFFECT:
            for (int i = 0; i < 3; ++i) {
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
            for (int j = 0; j < 2; ++j) {
                digitalWrite(INDICATOR_BACK_RIGHT, HIGH);
                digitalWrite(INDICATOR_FRONT_RIGHT, HIGH);
                digitalWrite(INDICATOR_BACK_LEFT, HIGH);
                digitalWrite(INDICATOR_FRONT_LEFT, HIGH);
                digitalWrite(RC_LIGHT, HIGH);
                digitalWrite(BRAKE_LIGHTS, HIGH);

                wait(seconds);

                digitalWrite(INDICATOR_BACK_RIGHT, LOW);
                digitalWrite(INDICATOR_FRONT_RIGHT, LOW);
                digitalWrite(INDICATOR_BACK_LEFT, LOW);
                digitalWrite(INDICATOR_FRONT_LEFT, LOW);
                digitalWrite(RC_LIGHT, LOW);
                digitalWrite(BRAKE_LIGHTS, LOW);

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

void LEDControl::wait(double seconds) {
    interval = seconds * 1000000;
    currentMillis = micros();
    while (micros() - currentMillis <= interval);
}