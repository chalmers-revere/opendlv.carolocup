#ifndef CAROLO_CUP_SENSORS_LIBRARY_H
#define CAROLO_CUP_SENSORS_LIBRARY_H

#include <Arduino.h>
#include <Wire.h>
#include "CurieIMU.h"

#define BAUD 115200

#define ID_IN_ULTRASONIC_CENTER     1
#define ID_IN_ULTRASONIC_CENTER_R   2
#define ID_IN_ULTRASONIC_SIDE_FRONT 3
#define ID_IN_ULTRASONIC_SIDE_BACK  4
#define ID_IN_ULTRASONIC_BACK       5
#define ID_IN_ENCODER_L             6
#define ID_IN_ENCODER_R             7
#define ID_IN_BUTTON_LANE           8
#define ID_IN_BUTTON_PARK           9
#define ID_IN_BUTTON_OVERTAKE       10
#define ID_IN_GX                    11
#define ID_IN_GY                    12
#define ID_IN_GZ                    13
#define ID_IN_AX                    14
#define ID_IN_AY                    15
#define ID_IN_AZ                    16
#define ID_IN_STEP                  17

#define US_FRONT 0x71 //front ultrasonic pin
#define US_FRONT_T 0x72 //front tilted right ultrasonic
#define US_BACK 0x75 //back ultrasonic pin
#define US_SIDE_FRONT 0x73 //side front right ultrasonic
#define US_SIDE_BACK 0x74 //side back ultrasonic pin

#define ENCODER_A 2 //wheel encoder channel A pin
#define ENCODER_B 3 //wheel encoder channel B pin

#define BUTTON_PARK 4
#define BUTTON_LANE 5
#define BUTTON_OVERTAKE 6

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

	int getDistance();

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

	int getStep();

	void readAccelerometer();

	int getAX();

	int getAY();

	int getAZ();

	void readGyro();

	int getGX();

	int getGY();

	int getGZ();

private:
	float ax, ay, az;   //scaled accelerometer values

	float gx, gy, gz; //scaled Gyro values
};

#endif
