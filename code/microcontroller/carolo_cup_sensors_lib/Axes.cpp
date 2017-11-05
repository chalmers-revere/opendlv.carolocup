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
		_ax = convertRawAcceleration(ax);
		_ay = convertRawAcceleration(ay);
		_az = convertRawAcceleration(az);
		_gx = convertRawGyro(gx);
		_gy = convertRawGyro(gy);
		_gz = convertRawGyro(gz);

		// update the filter, which computes orientation
		filter.updateIMU(_gx, _gy, _gz, _ax, _ay, _az);

		roll = (int) filter.getRoll();
		pitch = (int) filter.getPitch();
		yaw = (int) filter.getYaw();

		// increment previous time, so we keep proper pace
		microsPrevious = microsPrevious + microsPerReading;
	}

	//CurieIMU.readGyro(gx, gy, gz);
}

float Axes::convertRawAcceleration(int aRaw) {
	// since we are using 2G range
	// -2g maps to a raw value of -32768
	// +2g maps to a raw value of 32767

	float a = (aRaw * 2.0) / 32768.0;
	return a;
}

float Axes::convertRawGyro(int gRaw) {
	// since we are using 250 degrees/seconds range
	// -250 maps to a raw value of -32768
	// +250 maps to a raw value of 32767

	float g = (gRaw * 250.0) / 32768.0;
	return g;
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
