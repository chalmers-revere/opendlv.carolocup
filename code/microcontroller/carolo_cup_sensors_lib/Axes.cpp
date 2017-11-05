#include "CaroloCupSensors.h"

Axes::Axes()
{
	ax = 0;
	ay = 0;
	az = 0;   //scaled accelerometer values

	gx = 0;
	gy = 0;
	gz = 0;   //scaled gyro values
}

void Axes::begin()
{
	// intialize the sensor:
	CurieIMU.begin();

	CurieIMU.autoCalibrateGyroOffset();
	CurieIMU.autoCalibrateAccelerometerOffset(X_AXIS, 0);
	CurieIMU.autoCalibrateAccelerometerOffset(Y_AXIS, 0);
	CurieIMU.autoCalibrateAccelerometerOffset(Z_AXIS, 0);

	// Set the accelerometer range to 2G
	//CurieIMU.setAccelerometerRange(2);

	// Set the accelerometer range to 250 degrees/second
	//CurieIMU.setGyroRange(250);
}

void Axes::readMotion()
{
	//  read the accelerometer and gyroscope using this function.
	CurieIMU.readMotionSensor(ax, ay, az, gx, gy, gz);
}

int Axes::getAX()
{ return (int) ax; }

int Axes::getAY()
{ return (int) ay; }

int Axes::getAZ()
{ return (int) az; }

int Axes::getGX()
{ return (int) gx; }

int Axes::getGY()
{ return (int) gy; }

int Axes::getGZ()
{ return (int) gz; }
