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

protocol_data data_motor;
protocol_data data_servo;
protocol_data data_LEDS;

unsigned long currentMillis;
unsigned long interval;

unsigned long _blink = 0;

volatile int interrupt = 0;
volatile int rcControllerFlag = 0;

unsigned long oldMillis;
volatile int noData = 0;
int oldNoData = 0;

volatile int gotData = 0;

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

	if (!interrupt)
	{
		//ledControl.setIndicators(ID_OUT_INDICATOR_RF, 0.2);
#ifdef RUN
		timeout();
#endif
		gotData = 0;

		data_motor.id = 0;
		data_motor.value = 90;

		data_servo.id = 0;
		data_servo.value = 90;

		data_LEDS.id = -1;
		data_LEDS.sub_id = -1;

		while (Serial.available() && !gotData && !noData)
		{
			uint8_t incoming = Serial.read();
			uint8_t id = protocol.decodeOneByte(incoming);

			switch (id)
			{
				case ID_OUT_LIGHTS:
					data_LEDS = protocol.getData();
					gotData = 1;
					break;
				case ID_OUT_MOTOR:
					data_motor = protocol.getData();
					gotData = 1;
					break;
				case ID_OUT_SERVO:
					data_servo = protocol.getData();
					gotData = 1;
					break;
				default:
					break;
			}
		}

		if (!noData)
		{
			if (data_motor.id == ID_OUT_MOTOR)
			{
				ledControl.setBrakeLights(_OFF_);
				esc.setSpeed(data_motor.value);
#ifdef DEBUG
				Serial.print("Motor ");
				Serial.println(data_motor.value);
#endif
			}

			if (data_servo.id == ID_OUT_SERVO)
			{
				servo.setAngle(data_servo.value);
#ifdef DEBUG
				Serial.print("Servo ");
				Serial.println(data_servo.value);
#endif
			}

			if (data_LEDS.id == ID_OUT_LIGHTS)
			{
				if (data_LEDS.sub_id == ID_OUT_BRAKE)
				{
					esc.brake();
					ledControl.setBrakeLights(_ON_);
				}
				else if (data_LEDS.sub_id >= ID_OUT_LIGHTS_EFFECT && data_LEDS.sub_id <= ID_OUT_INDICATOR_RB)
				{
					ledControl.setIndicators(data_LEDS.sub_id, 0.5);
				}

#ifdef DEBUG
				Serial.print("LED ");
				Serial.println(data_LEDS.sub_id);
#endif
			}
		}
		else if (noData && (oldNoData != noData) && !interrupt)
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
	}

#ifdef DEBUG
	//Serial.println(receiver.readChannel1());
	//Serial.println(receiver.readChannel2());
#endif

#ifdef RUN
	oldNoData = noData;
	noData = 0;
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
	if (interrupt)
	{
		Serial.read();
	}
}
