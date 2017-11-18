#define SCSS_RSTC 0x570
#define RSTC_WARM_RESET (1 << 1)

void reboot(void) //function to do software reboot on arduino if necessary
{
	SCSS_REG_VAL(SCSS_SS_CFG) |= ARC_HALT_REQ_A;
	SCSS_REG_VAL(SCSS_RSTC) = RSTC_WARM_RESET;
}


unsigned long a = 0;
volatile int rcControllerFlag = 0;

void setup()
{
	pinMode(2, INPUT);

	Serial.begin(115200);

	attachInterrupt(2, interrupt, CHANGE);
}

void loop()
{
	Serial.println("Main loop");
}

void interrupt(void)
{
	a = pulseMeasure(2);

	if (a >= 1450 && a <= 1550)
	{
		rcControllerFlag++;
	}
	else
	{
		rcControllerFlag = 0;
	}

	if (rcControllerFlag >= 3)
	{
		Serial.println("Interrupt");
	}
}

unsigned long pulseMeasure(uint8_t pin)
{

	uint8_t state = HIGH;
	unsigned long pulseWidth = 0;
	unsigned long loopCount = 0;
	unsigned long loopMax = 25000;

	// While the pin is *not* in the target state we make sure the timeout hasn't been reached.

	Serial.println("Pulse start");

	while ((digitalRead(pin)) != state)
	{
		if (loopCount++ == loopMax)
		{
			return 0;
		}
	}
	// When the pin *is* in the target state we bump the counter while still keeping track of the timeout.
	while ((digitalRead(pin)) == state)
	{
		if (loopCount++ == loopMax)
		{
			return 0;
		}
		pulseWidth++;
	}

	Serial.println("Pulse end");

	// Return the pulse time in microsecond!
	return pulseWidth * 1.45; // Calculated the pulseWidth++ loop to be about 1.50uS in length.
}