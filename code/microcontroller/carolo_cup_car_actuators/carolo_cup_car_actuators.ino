/*
 * Comment it to turn off
 */
#define DEBUG
//#define RUN

#include "CaroloCupActuators.h"
#include "Protocol.h"

Protocol protocol;
SteeringMotor servo;
ESCMotor esc;
RCReceiver receiver;
LEDControl ledControl;

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
//		Serial.print("ch1 ");
//		Serial.println(a);
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
			servo.setAngle(STRAIGHT_DEGREES);
			rcControllerFlag = 0;
			ledControl.setRCLight(0, _blink);
			interrupt = 0;
			wait(1);
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
	if (!interrupt)
	{
		timeout();
	}
#endif

#ifdef DEBUG
	//Serial.println(receiver.readChannel1());
	//Serial.println(receiver.readChannel2());
#endif

#ifdef RUN

#endif
}

void establishContact(char toSend, int st)
{
	unsigned int s = 1;
	while (Serial.available() <= 0)
	{
		Serial.write(toSend);   // send a char
		ledControl.setStatusLight(s);
		wait(0.5);
		s = !s;
	}
	if (!st)
	{
		Serial.read();
	}
	esc.arm();
	ledControl.setIndicators(LED_SIGNAL, 0.5); //blink all leds to aware car is on

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

void serialEvent()
{
	if (!interrupt)
	{
//		ledControl.setIndicators(INDICATOR_FRONT_RIGHT, 0.5);
//		ledControl.setIndicators(INDICATOR_FRONT_LEFT, 0.5);

		uint8_t incoming = Serial.read();
		noData = 0;
		uint8_t id = protocol.decodeOneByte(incoming);
		if (id)
		{
			int value = protocol.getValue();
#ifdef DEBUG
			Serial.print("not 0 ");
			Serial.println(id);
			Serial.println(value);
#endif
			switch (protocol.getId())
			{
				case ID_OUT_MOTOR:
					ledControl.setBrakeLights(_OFF_);
					esc.setSpeed(value);
					break;
				case ID_OUT_SERVO:
					servo.setAngle(value);
					break;
				default:
					break;
			}
		}
		else
		{
			//int value = protocol.getValue();
#ifdef DEBUG
			Serial.print("is 0 ");
			Serial.println(protocol.getSubId());
#endif
			if (protocol.getSubId() == ID_OUT_BRAKE)
			{
				esc.brake();
				ledControl.setBrakeLights(_ON_);
			}
			else if (protocol.getSubId() >= ID_OUT_LIGHTS_EFFECT && protocol.getSubId() <= ID_OUT_INDICATOR_RB) {
				ledControl.setIndicators(protocol.getSubId(), 0.5);
			}
		}
	}
	else
	{
		Serial.read();
	}
}

