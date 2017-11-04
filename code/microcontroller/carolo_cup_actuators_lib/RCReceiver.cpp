#include "CaroloCupActuators.h"

RCReceiver::RCReceiver()
{
	ch1 = MIDDLE;
	ch2 = MIDDLE;
}

void RCReceiver::begin()
{
	pinMode(CH_1, INPUT);
	pinMode(CH_2, INPUT);
}

int RCReceiver::readChannel1()
{
	ch1 = pulseIn(CH_1, HIGH); //PULSE_TIMEOUT maybe add timeout
	return  ch1;
}

int RCReceiver::readChannel2()
{
	ch2 = pulseIn(CH_2, HIGH); //PULSE_TIMEOUT maybe add timeout
	return ch2;
}

int RCReceiver::filter(int val)
{
	//trim values
	int filtered = val;
	if (val == 0) //zero is also considered as the center value
	{
		filtered = MIDDLE;
	}
	else if (val <= LO) //Trim Noise from bottom end
	{
		filtered = LO;
	}
	else if (val <= DEAD_HIGH && val >= DEAD_LOW) //Create Dead-Band
	{
		filtered = MIDDLE;
	}
	else if (val >= HI) //Trim Noise from top end
	{
		filtered = HI;
	}
	//end of trimming

	//convert to range 0 to 180
	if (filtered >= LO && filtered <= DEAD_LOW)
	{
		filtered = map(filtered, LO, DEAD_LOW, FULL_BACKWARD, IDLE_SPEED);
	}
	else if (filtered == MIDDLE)
	{
		filtered = IDLE_SPEED;
	}
	else if (filtered >= DEAD_HIGH && filtered <= HI)
	{
		filtered = map(filtered, DEAD_HIGH, HI, IDLE_SPEED, FULL_FORWARD);
	}
	//end of convertion

	return filtered;
}
