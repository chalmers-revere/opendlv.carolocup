#include "CaroloCupActuators.h"

ESCMotor::ESCMotor() {
    _speed = IDLE_SPEED;
}

void ESCMotor::init() {
    _pin = ESC_PIN;
    attach(_pin); //attach the servo to its pin
    arm();
}

void ESCMotor::arm() {
    write(IDLE_SPEED);
}

void ESCMotor::setSpeed(int speed) { //receives a speed in the scale of 0 to 180
    speed = constrain(speed, FULL_BACKWARD, FULL_FORWARD);
    if (speed != _speed) {
        _speed = speed;
#ifdef DEBUG
        Serial.print("esc ");
        Serial.println(_speed);
#endif
        write(_speed); //write the appropriate pwm signal to the servo
    }
}

void ESCMotor::brake() {
#ifdef DEBUG
    Serial.print("_sp ");
    Serial.println(_speed);
#endif
    if (_speed < IDLE_SPEED) {
        write(IDLE_SPEED + 15);
#ifdef DEBUG
        Serial.println("b1");
#endif
    } else if (_speed > IDLE_SPEED) {
        write(IDLE_SPEED - 55);
#ifdef DEBUG
        Serial.println("b2");
#endif
    }
    write(IDLE_SPEED);
    _speed = IDLE_SPEED;
}
