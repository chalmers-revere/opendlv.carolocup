#include "term4_car_lib.h"

ESCMotor::ESCMotor() {
}

void ESCMotor::setFreqsAndSpeeds() {
    FULL_FORWARD = 180;
    FULL_BACKWARD = 0;
    IDLE_SPEED = 90;
    IDLE_RAW_SPEED = 1500;
    MAX_FRONT_RAW_SPEED = 2000;
    MAX_BACK_RAW_SPEED = 1000;
    _speed = IDLE_SPEED;
}

void ESCMotor::init(unsigned short pin) {
    _pin = pin;
    _direction = 1;
    setFreqsAndSpeeds();
    attach(_pin); //attach the servo to its pin
    arm();
}

void ESCMotor::arm() {
    write(IDLE_SPEED);
}

void ESCMotor::setSpeed(int speed) { //receives a speed in the scale of 0 to 180
    int s = IDLE_SPEED;
    if (speed <= FULL_FORWARD && speed >= FULL_BACKWARD) {
        s = indicateDirection(speed);
    } else {
        s = filterSpeed(speed);
    }
    if (s != _speed) {
        _speed = s;
        write(_speed); //write the appropriate pwm signal to the servo
    }
}

void ESCMotor::brake() {
    red = 255;
    green = 0;
    blue = 0;
#ifdef COMMON_ANODE
    red = 255 - red;
    green = 255 - green;
    blue = 255 - blue;
#endif
    analogWrite(redPin, red);
    analogWrite(greenPin, green);
    analogWrite(bluePin, blue);

    if (_direction && _speed != IDLE_SPEED) {
        write(IDLE_SPEED + 15);
    } else if (_speed != IDLE_SPEED && !_direction) {
        write(IDLE_SPEED - 55);
    }
    write(IDLE_SPEED);
    _speed = IDLE_SPEED;
}

int ESCMotor::filterSpeed(int speed) {
    int filtered = speed;
    if (speed == 0) //zero is also considered as the center value
    {
        filtered = center;
    } else if (speed <= lo) //Trim Noise from bottom end
    {
        filtered = lo;
    } else if (speed <= deadHi && speed >= deadLo) //Create Dead-Band
    {
        filtered = center;
    } else if (speed >= hi) //Trim Noise from top end
    {
        filtered = hi;
    }

    if (filtered >= lo && filtered <= deadLo) {
        filtered = map(filtered, lo, deadLo, FULL_BACKWARD + 25, IDLE_SPEED - 1);
    } else if (filtered == center) {
        filtered = IDLE_SPEED;
    } else if (filtered >= deadHi && filtered <= hi) {
        filtered = map(filtered, deadHi, hi, IDLE_SPEED + 1, FULL_FORWARD - 30);
    }
    filtered = indicateDirection(filtered);
    return filtered;
}

int ESCMotor::indicateDirection(int speed) {
    if (speed >= FULL_BACKWARD && speed < IDLE_SPEED) {
        _direction = 0;
        red = 255;//set
        green = 0;
        blue = 255;
#ifdef COMMON_ANODE
        red = 255 - red;
        green = 255 - green;
        blue = 255 - blue;
#endif
        analogWrite(redPin, red);
        analogWrite(greenPin, green);
        analogWrite(bluePin, blue);
        return speed;
    } else if (speed > IDLE_SPEED && speed <= FULL_FORWARD) {
        _direction = 1;
        red = 255;//set
        green = 255;
        blue = 0;
#ifdef COMMON_ANODE
        red = 255 - red;
        green = 255 - green;
        blue = 255 - blue;
#endif
        analogWrite(redPin, red);
        analogWrite(greenPin, green);
        analogWrite(bluePin, blue);
        return speed;
    }
    return speed;
}

void ESCMotor::wait(double seconds) {
    interval = seconds * 1000;
    currentMillis = millis();
    while (millis() - currentMillis <= interval);
}
