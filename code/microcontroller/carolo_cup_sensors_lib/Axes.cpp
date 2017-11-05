#include "CaroloCupSensors.h"

Axes::Axes()
{
	ax = 0;
	ay = 0;
	az = 0;   //scaled accelerometer values

	gx = 0;
	gy = 0;
	gz = 0;   //scaled gyro values

	yaw = 0;
	pitch = 0;
	roll = 0;

	factor = 800; // variable by which to divide gyroscope values, used to control sensitivity
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

	//CurieImu.gyroOffsetEnabled();
}

void Axes::readMotion()
{
	//  read the accelerometer and gyroscope using this function.
	//CurieIMU.readMotionSensor(ax, ay, az, gx, gy, gz);
	CurieImu.readGyro(gx, gy, gz);

	filter.updateIMU(gx/factor, gy/factor, gz/factor, 0.0, 0.0, 0.0);

	// functions to find yaw roll and pitch from quaternions
	yaw = int(filter.getYaw()*100);
	roll = int(filter.getRoll()*100);
	pitch = int(filter.getPitch()*100);
}

int Axes::getAX()
{ return ax; }

int Axes::getAY()
{ return ay; }

int Axes::getAZ()
{ return az; }

int Axes::getGX()
{ return yaw; }

int Axes::getGY()
{ return pitch; }

int Axes::getGZ()
{ return roll; }
