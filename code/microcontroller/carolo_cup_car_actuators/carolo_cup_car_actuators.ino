/*
 * Comment it to turn off
 */
//#define DEBUG
#define RUN

#include "CaroloCupActuators.h"
#include "Protocol.h"

Protocol protocol;
SteeringMotor servo;
ESCMotor esc;
RCReceiver receiver;
LEDControl ledControl;
Axes axes;

unsigned long currentMillis;
unsigned long interval;

unsigned long _blink = 0;

volatile int interrupt = 0;
volatile int rcControllerFlag = 0;

unsigned long oldMillis;
int noData = 0;
int oldNoData = 0;

#define SCSS_RSTC 0x570
#define RSTC_WARM_RESET (1 << 1)

void reboot(void) //function to do software reboot on arduino if necessary
{
	SCSS_REG_VAL(SCSS_SS_CFG) |= ARC_HALT_REQ_A;
	SCSS_REG_VAL(SCSS_RSTC) = RSTC_WARM_RESET;
}

void setup()
{
	axes.begin();
	servo.init();
	esc.init();
	receiver.begin();
	ledControl.begin();

	ledControl.setIndicators(LED_SIGNAL, 0.5); //blink all leds to aware car is on

	Serial.begin(BAUD);

#ifdef RUN
	waitConnection();

	establishContact('a', 0);
#endif
}

void loop()
{
	if (!interrupt)
	{
		int a = receiver.readChannel1();

#ifdef DEBUG
		Serial.print("ch1 ");
		Serial.println(a);
#endif

		if (a >= DEAD_LOW && a <= DEAD_HIGH)
		{
			rcControllerFlag++;
		}
		else
		{
			rcControllerFlag = 0;
		}

		if (rcControllerFlag >= 3)
		{
			esc.brake();
			esc.arm();
			ledControl.setBrakeLights(_ON_);
		}
	}

	_blink++;
	if (_blink > 1000000) _blink = 0;

	if (rcControllerFlag >= 3)
	{
		ledControl.setRCLight(35, _blink);

		if (!interrupt)
		{
			esc.arm();
			wait(2);
		}

		interrupt = 1;

		int angle = receiver.readChannel1();
		int speed = receiver.readChannel2();

		if (angle == 0)
		{
			esc.brake();
			ledControl.setBrakeLights(_ON_);

			rcControllerFlag = 0;
			ledControl.setRCLight(0, _blink);
			interrupt = 0;
			return;
		}

		ledControl.setBrakeLights(_OFF_);
		servo.setAngle(receiver.filter(angle));
		esc.setSpeed(receiver.filter(speed));
#ifdef DEBUG
		Serial.print("steer ");
		Serial.println(receiver.filter(angle));
		Serial.print("speed ");
		Serial.println(receiver.filter(speed));
#endif
	}

	if (noData && (oldNoData != noData) && !interrupt)
	{
		servo.setAngle(STRAIGHT_DEGREES);
		esc.brake();
		esc.arm();
		ledControl.setBrakeLights(_ON_);
		establishContact('a', 1);
#ifdef DEBUG
		Serial.println("TIMEOUT");
#endif
	}

	oldNoData = noData;

#ifdef RUN
	if (!interrupt) {
	timeout();
}
#endif

	axes.readMotion();
#ifdef DEBUG
	Serial.print("YAW ");
	Serial.println(axes.getYaw());
	//Serial.println(receiver.readChannel1());
	//Serial.println(receiver.readChannel2());
#endif

#ifdef RUN
	encodeAndWrite(ID_IN_YAW, axes.getYaw());
#endif
}

void establishContact(char toSend, int st)
{
	while (Serial.available() <= 0)
	{
		Serial.write(toSend);   // send a char
		wait(0.5);
	}
	if (!st) {
		Serial.read();
		wait(5);
		esc.arm();
		ledControl.setIndicators(LED_SIGNAL, 0.5); //blink all leds to aware car is on
		//ledControl.setHeadLights(_ON_);
	} else {
		esc.arm();
		ledControl.setIndicators(LED_SIGNAL, 0.5); //blink all leds to aware car is on
	}
}

void waitConnection()
{
	while (!Serial); // wait for serial port to connect. Needed for native USB port only
}

void wait(double seconds)
{
	interval = seconds * 1000000;
	currentMillis = micros();
	while (micros() - currentMillis <= interval);
}

void timeout()
{
	oldMillis = micros();
	while (!Serial.available())
	{

		if ((micros() - oldMillis) > T_OUT)
		{
			noData = 1;
			break;
		}
	}
}

void encodeAndWrite(int id, int value)
{
	int st = protocol.encode(id, value);

	if (st)
	{
		Serial.write(protocol.getBufferOut(), BUFFER_SIZE); //try this first
	}
}

void serialEvent()
{
	if (!interrupt)
	{
		uint8_t incoming = Serial.read();
		noData = 0;
		protocol.decode(incoming);
		if (protocol.isValid())
		{
			int value = protocol.getValue();
			switch (protocol.getId())
			{
				case ID_OUT_BRAKE:
					esc.brake();
					ledControl.setBrakeLights(_ON_);
					break;
				case ID_OUT_MOTOR:
					ledControl.setBrakeLights(_OFF_);
					esc.setSpeed(value);
					break;
				case ID_OUT_SERVO:
					servo.setAngle(value);
					break;
				case ID_OUT_INDICATORS:
					ledControl.setIndicators(value, 0.5);
					break;
				default:
					break;
			}
		}
	}
	else
	{
		Serial.read();
	}
}

unsigned long pulseMeasure(uint8_t pin)
{

	uint8_t state = HIGH;
	unsigned long pulseWidth = 0;
	unsigned long loopCount = 0;
	unsigned long loopMax = 5000000;

	// While the pin is *not* in the target state we make sure the timeout hasn't been reached.
#ifdef DEBUG
	Serial.println("Pulse start");
#endif
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
#ifdef DEBUG
	Serial.println("Pulse end");
#endif
	// Return the pulse time in microsecond!
	return pulseWidth * 1.45; // Calculated the pulseWidth++ loop to be about 1.50uS in length.
}

