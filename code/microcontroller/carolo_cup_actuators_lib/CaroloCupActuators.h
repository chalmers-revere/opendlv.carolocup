#pragma clang diagnostic push
#pragma ide diagnostic ignored "CannotResolve"
#ifndef CAROLO_CUP_ACTUATORS_LIBRARY_H
#define CAROLO_CUP_ACTUATORS_LIBRARY_H

#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>
#include <Adafruit_PWMServoDriver.h>

#define BAUD 115200

#define ID_OUT_BRAKE                0
#define ID_OUT_MOTOR                1
#define ID_OUT_SERVO                2
#define ID_OUT_LIGHTS_EFFECT        3
#define ID_OUT_INDICATORS           4
#define ID_OUT_INDICATOR_LF         6
#define ID_OUT_INDICATOR_RF         7
#define ID_OUT_INDICATOR_LB         8
#define ID_OUT_INDICATOR_RB         10

#define SERVO_PIN 0 //steering servo pin
#define _SERVO_PIN 11 //steering servo pin
#define ESC_PIN 9 //ESC motor pin

#define CH_1 2 //A0 //RC receiver channel 1 pin (steer)
#define CH_2 A1 //RC receiver channel 2 pin (drive)

#define BRAKE_LIGHTS 4
#define RC_LIGHT 5
#define INDICATOR_FRONT_LEFT 6
#define INDICATOR_FRONT_RIGHT 7
#define INDICATOR_BACK_LEFT 8
#define INDICATOR_BACK_RIGHT 10
#define HEADLIGHTS 12

#define PULSE_TIMEOUT 25000 //10000

#define T_OUT 5000

//Signal Conditioning limits for RC controller
#define LO 1090
#define MIDDLE 1500
#define HI 1965
#define DEAD_LOW 1450
#define DEAD_HIGH 1550

//servo 308
#define SERVOMIN  169//171 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  500//530 // this is the 'maximum' pulse length count (out of 4096)
#define MAX_RIGHT_ANGLE 180
#define MAX_LEFT_ANGLE 0
#define STRAIGHT_DEGREES 90

//what percentage of the motor's power is allowed be used at most
#define FULL_FORWARD 180
#define FULL_BACKWARD 0
#define IDLE_SPEED 90

#define _OFF_ 0
#define _ON_ 1

class SteeringMotor : public Servo
{
public:
	explicit SteeringMotor();

	void begin(unsigned int min, unsigned int max);

	void init();

	int convertPulse(int in, int min, int max);

	void setAngle(int angle);

	void setDegrees(int angle);

private:
	Adafruit_PWMServoDriver pwm;

	unsigned int _angle, _min_pulse, _max_pulse;
};

class ESCMotor : public Servo
{
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

class LEDControl
{
public:
	explicit LEDControl();
	void begin();
	void setIndicators(unsigned int state, unsigned int frequency);
	void setBrakeLights(unsigned int value);
	void setHeadLights(unsigned int state);
	void setRCLight(unsigned int frequency, unsigned long counter);
	void wait(long milliseconds);
private:
	unsigned long currentMillis;
	long interval;
};

class RCReceiver
{
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
#pragma clang diagnostic pop