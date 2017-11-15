#include "CaroloCupSensors.h"

Axes::Axes()
{
	lastStreamTime = 0;

	yaw = 0;
	pitch = 0;
	roll = 0;

}

void Axes::begin()
{
	I2C.begin();                    //Initialize I2C communication to the let the library communicate with the sensor.
	//Sensor Initialization
	mySensor.initSensor();          //The I2C Address can be changed here inside this function in the library
	mySensor.setOperationMode(OPERATION_MODE_NDOF);   //Can be configured to other operation modes as desired
	mySensor.setUpdateMode(MANUAL);    //The default is AUTO. Changing to MANUAL requires calling the relevant update functions prior to calling the read functions
	//Setting to MANUAL requires fewer reads to the sensor
}

void Axes::readMotion()
{
	if ((micros() - lastStreamTime) >= streamPeriod)
	{
		lastStreamTime = micros();
		mySensor.updateEuler();        //Update the Euler data into the structure of the object
		mySensor.updateCalibStatus();  //Update the Calibration Status

		yaw = mySensor.readEulerHeading();

		roll = mySensor.readEulerRoll();

		pitch = mySensor.readEulerPitch();
	}
}

int Axes::getYaw()
{ return yaw; }

int Axes::getPitch()
{ return pitch; }

int Axes::getRoll()
{ return roll; }
