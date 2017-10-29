#pragma clang diagnostic push
#pragma ide diagnostic ignored "CannotResolve"
#ifndef CAROLOCUPCAR_H
#define CAROLOCUPCAR_H

#include <Arduino.h>
#include <Wire.h>
#include "CurieIMU.h"

#define BAUD 115200

#define US_FRONT 0x71 //front ultrasonic pin
#define US_FRONT_T 0x72 //front tilted right ultrasonic
#define US_BACK 0x75 //back ultrasonic pin
#define US_SIDE_FRONT 0x73 //side front right ultrasonic
#define US_SIDE_BACK 0x74 //side back ultrasonic pin

#define ENCODER_A 2 //wheel encoder channel A pin
#define ENCODER_B 3 //wheel encoder channel B pin

#define WHEEL_CIRCUMFERENCE 18 //change this after accurate measurement of wheel circumference

class UltrasonicSensor
{
public:
	explicit UltrasonicSensor();

	void attach(unsigned short address);

	void setGain(unsigned short gainValue);

	void setRange(unsigned short rangeValue);

	void setPingDelay(unsigned short milliseconds = DEFAULT_PING_DELAY);

	unsigned int getDistance();

	void changeAddress(unsigned short newAddress);

	void encodeAndWrite(int id, int value);

	void wait(long milliseconds);

private:
	uint8_t _ADDRESS;
	unsigned short _DELAY;
	static const unsigned short DEFAULT_SRF08_ADDRESS;
	const unsigned short MIN_PING_DELAY;
	unsigned long currentMillis;
	long interval;
};

class Odometer
{
public:
	explicit Odometer();

	void begin();

	unsigned long getDistance();

	float getSpeed();

	void resetWheelCounters();

	void encodeAndWrite(int id, int value);

private:
	unsigned long distanceTraveledLeftWheel;
	unsigned long distanceTraveledRightWheel;
	unsigned long rpmReadingLeftWheel;
	unsigned long rpmReadingRightWheel;
	unsigned long timeOld;
	unsigned long wheelCircumference;
};

class Axes
{
public:
	explicit Axes();

	void begin();

	long getStep();

	void readAccelerometer();

	float getAX();

	float getAY();

	float getAZ();

	void readGyro();

	float getGX();

	float getGY();

	float getGZ();

private:
	float ax, ay, az;   //scaled accelerometer values

	float gx, gy, gz; //scaled Gyro values
};


#endif
#pragma clang diagnostic pop