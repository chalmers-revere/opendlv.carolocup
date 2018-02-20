#include "CaroloCupSensors.h"
#include "Protocol.h"

//#define RUN // uncomment this for normal executation and comment DEBUG
#define DEBUG //uncomment this for debugging and comment RUN

#define BOUNCE_DURATION 700   // define an appropriate bounce time in ms for your switches

unsigned long currentMillis;
unsigned long interval;

Protocol protocol;
Odometer odometer;
UltrasonicSensor u_right_front, u_right_back, u_back;
Axes axes;

volatile unsigned long bounceTimeP = 0; // variable to hold ms count to debounce a pressed switch
volatile unsigned long bounceTimeL = 0; // variable to hold ms count to debounce a pressed switch
volatile unsigned long bounceTimeO = 0; // variable to hold ms count to debounce a pressed switch

volatile int p = 0;
volatile int l = 0;
volatile int o = 0;

void setup()
{
	axes.begin();
	odometer.begin();

	u_right_front.attach(US_SIDE_FRONT);
	u_right_back.attach(US_SIDE_BACK);
	u_back.attach(US_BACK);

#ifdef RUN
	pinMode(BUTTON_PARK, INPUT);
	pinMode(BUTTON_LANE, INPUT);
	pinMode(BUTTON_OVERTAKE, INPUT);

	pciSetup(BUTTON_PARK);
	pciSetup(BUTTON_LANE);
	pciSetup(BUTTON_OVERTAKE);

	Serial.begin(BAUD);

	waitConnection();

	establishContact('s');
#endif
}

void loop()
{
	axes.readMotion();

#ifdef RUN
	encodeAndWrite(ID_IN_YAW, axes.getYaw());

	sendButtonsIDLE();

	encodeAndWrite(ID_IN_ULTRASONIC_SIDE_FRONT, u_right_front.getDistance());
	encodeAndWrite(ID_IN_ULTRASONIC_SIDE_BACK, u_right_back.getDistance());
	encodeAndWrite(ID_IN_ULTRASONIC_BACK, u_back.getDistance());

	encodeAndWrite(ID_IN_ENCODER_L, odometer.getDistance());
#endif

#ifdef DEBUG
//	Serial.println(u_center.getDistance());
//	Serial.println(u_front_right.getDistance());
  	Serial.println(u_right_front.getDistance());
	Serial.println(u_right_back.getDistance());
	Serial.println(u_back.getDistance());
	Serial.println(odometer.getDistance());
  	Serial.print("YAW ");
  	Serial.println(axes.getYaw());
#endif
}

void encodeAndWrite(int id, int value)
{
	int st = protocol.encode(id, value);

	if (st)
	{
		Serial.write(protocol.getBufferOut(), BUFFER_SIZE);
	}
}

void establishContact(char toSend)
{
	while (Serial.available() <= 0)
	{
		Serial.write(toSend);   // send a char
		wait(0.5);
	}
	Serial.read();
}

void waitConnection()
{
	while (!Serial); // wait for serial port to connect. Needed for native USB port only
}

void wait(double seconds)
{
	interval = seconds * 1000;
	currentMillis = millis();
	while (millis() - currentMillis <= interval);
}

void sendButtonsIDLE()
{
	encodeAndWrite(ID_IN_BUTTON_PARK, 2);
	encodeAndWrite(ID_IN_BUTTON_LANE, 2);
	encodeAndWrite(ID_IN_BUTTON_OVERTAKE, 2);
}

void parkInterrupt()
{
	if (abs(millis() - bounceTimeP) > BOUNCE_DURATION)
	{
#ifdef DEBUG
		Serial.println("PARK");
#endif
		p = !p;
		encodeAndWrite(ID_IN_BUTTON_PARK, p);

		bounceTimeP = millis();  // set whatever bounce time in ms is appropriate
	}
}

void laneInterrupt()
{
	if (abs(millis() - bounceTimeL) > BOUNCE_DURATION)
	{
#ifdef DEBUG
		Serial.println("LANE");
#endif
		l = !l;
		encodeAndWrite(ID_IN_BUTTON_LANE, l);

		bounceTimeL = millis();  // set whatever bounce time in ms is appropriate
	}
}

void overtakeInterrupt()
{
	if (abs(millis() - bounceTimeO) > BOUNCE_DURATION)
	{
#ifdef DEBUG
		Serial.println("OVER");
#endif
		o = !o;
		encodeAndWrite(ID_IN_BUTTON_OVERTAKE, o);

		bounceTimeO = millis();  // set whatever bounce time in ms is appropriate
	}
}

void pciSetup(byte pin)
{
	*digitalPinToPCMSK(pin) |= bit(digitalPinToPCMSKbit(pin));  // enable pin
	PCIFR |= bit(digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
	PCICR |= bit(digitalPinToPCICRbit(pin)); // enable interrupt for the group
}

ISR (PCINT0_vect) // handle pin change interrupt for D8 to D13 here
		{
				if
				(digitalRead(BUTTON_LANE)) {
					laneInterrupt();
				} else if (digitalRead(BUTTON_PARK)) {
					parkInterrupt();
				} else if (digitalRead(BUTTON_OVERTAKE)) {
					overtakeInterrupt();
				}
		}
