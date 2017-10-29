#include "CaroloCupSensors.h"

boolean stepEventsEnabeled = true;   // whether you're polling or using events
volatile long lastStepCount = 0;              // step count on previous polling check

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
	// turn on step detection mode:
	CurieIMU.setStepDetectionMode(CURIE_IMU_STEP_MODE_NORMAL);
	// enable step counting:
	CurieIMU.setStepCountEnabled(true);

	if (stepEventsEnabeled)
	{
		// attach the eventCallback function as the
		// step event handler:
		CurieIMU.attachInterrupt(eventCallback);
		CurieIMU.interrupts(CURIE_IMU_STEP);  // turn on step detection
	}

	// Set the accelerometer range to 2G
	CurieIMU.setAccelerometerRange(2);

	// Set the accelerometer range to 250 degrees/second
	CurieIMU.setGyroRange(250);
}

long Axes::getStep()
{
	return lastStepCount;
}

/* Instead of using step detection event notifications,
   we can check the step count periodically */
static void updateStepCount()
{
	// get the step count:
	int stepCount = CurieIMU.getStepCount();

	// if the step count has changed, print it:
	if (stepCount != lastStepCount)
	{
		// save the current count for comparison next check:
		lastStepCount = stepCount;
	}
}

static void eventCallback(void)
{
	if (CurieIMU.stepsDetected())
		updateStepCount();
}

void Axes::readAccelerometer()
{
	// read accelerometer measurements from device, scaled to the configured range
	CurieIMU.readAccelerometerScaled(ax, ay, az);
}

float Axes::getAX()
{ return ax; }

float Axes::getAY()
{ return ay; }

float Axes::getAZ()
{ return az; }

void Axes::readGyro()
{
	// read gyro measurements from device, scaled to the configured range
	CurieIMU.readGyroScaled(gx, gy, gz);
}

float Axes::getGX()
{ return gx; }

float Axes::getGY()
{ return gy; }

float Axes::getGZ()
{ return gz; }
