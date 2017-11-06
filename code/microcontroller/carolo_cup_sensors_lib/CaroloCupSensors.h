#ifndef CAROLO_CUP_SENSORS_LIBRARY_H
#define CAROLO_CUP_SENSORS_LIBRARY_H

#include <Arduino.h>
#include <Wire.h>
#include "CurieIMU.h"
#include "MadgwickAHRS.h"

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
#define ID_IN_YAW                   11
#define ID_IN_ROLL                  12
#define ID_IN_PITCH                 13

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

#define DEFAULT_SRF08_ADDRESS 112
#define MIN_PING_DELAY 10
#define CLOCK_SPEED 400000
#define RANGE 24
#define GAIN 10

class UltrasonicSensor
{
public:
	explicit UltrasonicSensor();

	void attach(unsigned short address);

	void setGain(unsigned short gainValue);

	void setRange(unsigned short rangeValue);

	void setPingDelay(unsigned short milliseconds);

	unsigned int getDistance();

	void changeAddress(unsigned short newAddress);

	void wait(unsigned long milliseconds);

private:
	uint8_t _ADDRESS;
	unsigned short _DELAY;
	unsigned long currentMillis;
	unsigned long interval;
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

	void readMotion();

	int getYaw();

	int getPitch(); // if necessary protocol has to be modified to accept negative numbers

	int getRoll(); // if necessary protocol has to be modified to accept negative numbers

	float convertRawAcceleration(int aRaw);

	float convertRawGyro(int gRaw);

private:
	int ax, ay, az;   //scaled accelerometer values

	int gx, gy, gz; //scaled Gyro values

	Madgwick filter;

	int yaw;
	int pitch;
	int roll;

	int factor; // variable by which to divide gyroscope values, used to control sensitivity

	unsigned long microsPerReading, microsPrevious;
	float accelScale, gyroScale;
};

#endif
