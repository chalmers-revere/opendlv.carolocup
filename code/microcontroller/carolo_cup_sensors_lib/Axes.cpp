#include "CaroloCupSensors.h"

boolean stepEventsEnabeled = true;   // whether you're polling or using events
volatile long lastStepCount = 0;              // step count on previous polling check
volatile int steps = 0;

static void eventCallback(void)
{
	if (CurieIMU.stepsDetected())
		updateStepCount();
}

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

int Axes::getStep()
{
	return steps;
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
		steps = stepCount - lastStepCount;
		lastStepCount = stepCount;
	}
	else
	{
		steps = 0;
	}
}

void Axes::readAccelerometer()
{
	// read accelerometer measurements from device, scaled to the configured range
	CurieIMU.readAccelerometerScaled(ax, ay, az);
}

int Axes::getAX()
{ return (int) ax; }

int Axes::getAY()
{ return (int) ay; }

int Axes::getAZ()
{ return (int) az; }

void Axes::readGyro()
{
	// read gyro measurements from device, scaled to the configured range
	CurieIMU.readGyroScaled(gx, gy, gz);
}

int Axes::getGX()
{ return (int) gx; }

int Axes::getGY()
{ return (int) gy; }

int Axes::getGZ()
{ return (int) gz; }
