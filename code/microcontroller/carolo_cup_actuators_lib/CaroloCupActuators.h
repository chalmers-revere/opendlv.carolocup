#ifndef CAROLO_CUP_ACTUATORS_LIBRARY_H
#define CAROLO_CUP_ACTUATORS_LIBRARY_H

#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>

#define BAUD 115200

#define COMMON_ADDRESS 8

#define ID_OUT_LIGHTS               0
#define ID_OUT_MOTOR                1
#define ID_OUT_SERVO                2

#define ID_OUT_BRAKE                6
#define ID_OUT_LIGHTS_EFFECT        1
#define ID_OUT_INDICATOR_LF         2
#define ID_OUT_INDICATOR_RF         3
#define ID_OUT_INDICATOR_LB         4
#define ID_OUT_INDICATOR_RB         5

#define LED_SIGNAL 6

#define _SERVO_PIN 6 //steering servo pin
#define ESC_PIN 5 //ESC motor pin

#define CH_1 2 //RC receiver channel 1 pin (steer)
#define CH_2 3 //RC receiver channel 2 pin (drive)

#define BRAKE_LIGHTS 12
#define RC_LIGHT 13
#define INDICATOR_FRONT_LEFT 4
#define INDICATOR_FRONT_RIGHT 7
#define INDICATOR_BACK_LEFT 8
#define INDICATOR_BACK_RIGHT 10
#define STATUS_LIGHT A0

#define PULSE_TIMEOUT 25000 //10000

#define T_OUT 5000000

//Signal Conditioning limits for RC controller
#define LO 1090
#define MIDDLE 1500
#define HI 1965
#define DEAD_LOW 1430
#define DEAD_HIGH 1580

#define MAX_RIGHT_ANGLE 180
#define MAX_LEFT_ANGLE 0
#define STRAIGHT_DEGREES 90

//what percentage of the motor's power is allowed be used at most
#define FULL_FORWARD 180
#define FULL_BACKWARD 0
#define IDLE_SPEED 90

#define _OFF_ 0
#define _ON_ 1

class SteeringMotor : public Servo {
public:
    explicit SteeringMotor();

    void init();

    void setAngle(unsigned int angle);


private:
    unsigned int _angle;
};

class ESCMotor : public Servo {
public:
    explicit ESCMotor();

    void init();

    void setSpeed(int speed);

    void brake();

    void arm();

private:
    unsigned short _pin; //the pin the ESC is attached to

    int _speed;
};

class LEDControl {
public:
    explicit LEDControl();

    void begin();

    void setIndicators(unsigned int state, double seconds);

    void setBrakeLights(unsigned int value);

    void setStatusLight(unsigned int state);

    void setRCLight(unsigned int frequency, unsigned long counter);

    void wait(double seconds);

private:
    unsigned long currentMillis;
    unsigned long interval;
};

class RCReceiver {
public:
    explicit RCReceiver();

    void begin();

    int readChannel1();

    int readChannel2();

    int filter(int val);

private:
    int ch1;
    int ch2;
};

#endif