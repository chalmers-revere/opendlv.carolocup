#include "CaroloCupSensors.h"

/* ---- ODOMETER ---- */
void countRevolutionsLeftWheel(); //ISR for the odometer
void countRevolutionsRightWheel();

volatile unsigned long halfRevolutionsLeftWheel = 0;
volatile unsigned long halfRevolutionsRightWheel = 0;

Odometer::Odometer()
{}

void Odometer::begin()
{
	attachInterrupt(0, countRevolutionsLeftWheel, RISING);
	attachInterrupt(1, countRevolutionsRightWheel, RISING);
	digitalWrite(ENCODER_A, HIGH); //internal pull-up resistors
	digitalWrite(ENCODER_B, HIGH); //to make sure it works, with no need for external resistors hooked up to the circuit.

	rpmReadingLeftWheel = 0;
	rpmReadingRightWheel = 0;
	timeOld = 0;
	wheelCircumference = WHEEL_CIRCUMFERENCE;
}

int Odometer::getDistance()
{
	unsigned long left = wheelCircumference * (halfRevolutionsLeftWheel * 2);

	unsigned long right = wheelCircumference * (halfRevolutionsRightWheel * 2);

	unsigned long t = (left + right) / 2;

	halfRevolutionsLeftWheel = 0;
	halfRevolutionsRightWheel = 0;

	return t;
}

float Odometer::getSpeed()
{
	//Update RPM every 20 counts, increase this for better RPM resolution,
	//rpmReadingLeftWheel = 30*1000/(millis() - //timeOldLeftWheel)*halfRevolutionsLeftWheel;
//     timeOldLeftWheel = millis();
//     halfRevolutionsLeftWheel = 0;

	//     //Update RPM every 20 counts, increase this for better RPM resolution,
//     rpmReadingRightWheel = 30*1000/(millis() - timeOldRightWheel)*halfRevolutionsRightWheel;
//     timeOldRightWheel = millis();
//     halfRevolutionsRightWheel = 0;

	return 0;
}

//reference functions
void countRevolutionsLeftWheel()
{
	halfRevolutionsLeftWheel++;
	//Each rotation, this interrupt function is run twice
}

void countRevolutionsRightWheel()
{
	halfRevolutionsRightWheel++;
	//Each rotation, this interrupt function is run twice
}

void Odometer::encodeAndWrite(int id, int value)
{

}