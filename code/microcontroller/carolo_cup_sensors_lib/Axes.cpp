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

	// initialize variables to pace updates to correct rate
	microsPerReading = 1000000 / 25;
}

void Axes::begin()
{
	// intialize the sensor:
	CurieIMU.begin();
	CurieIMU.setGyroRate(25);
	CurieIMU.setAccelerometerRate(25);
	filter.begin(25);

	CurieIMU.autoCalibrateGyroOffset();

	CurieIMU.autoCalibrateAccelerometerOffset(X_AXIS, 0);
	CurieIMU.autoCalibrateAccelerometerOffset(Y_AXIS, 0);
	CurieIMU.autoCalibrateAccelerometerOffset(Z_AXIS, 0);

	// Set the accelerometer range to 2G
	CurieIMU.setAccelerometerRange(2);

	// Set the accelerometer range to 250 degrees/second
	CurieIMU.setGyroRange(250);

	microsPrevious = micros();
}

void Axes::readMotion()
{
	float _ax, _ay, _az;
	float _gx, _gy, _gz;
	// check if it's time to read data and update the filter
	microsNow = micros();
	if (microsNow - microsPrevious >= microsPerReading)
	{
		//  read the accelerometer and gyroscope using this function.
		CurieIMU.readMotionSensor(ax, ay, az, gx, gy, gz);

		// convert from raw data to gravity and degrees/second units
		_ax = convertRawAcceleration(aix);
		_ay = convertRawAcceleration(aiy);
		_az = convertRawAcceleration(aiz);
		_gx = convertRawGyro(gix);
		_gy = convertRawGyro(giy);
		_gz = convertRawGyro(giz);

		// update the filter, which computes orientation
		filter.updateIMU(_gx, _gy, _gz, _ax, _ay, _az);

		roll = (int) filter.getRoll();
		pitch = (int) filter.getPitch();
		heading = (int) filter.getYaw();
	}

	//CurieIMU.readGyro(gx, gy, gz);
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
