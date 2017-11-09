#include "CaroloCupSensors.h"
#include "Protocol.h"

//#define RUN // uncomment this for normal executation and comment DEBUG
//#define DEBUG //uncomment this for debugging and comment RUN
#define BOUNCE_DURATION 700   // define an appropriate bounce time in ms for your switches

unsigned long currentMillis;
unsigned long interval;

Protocol protocol;
Odometer odometer;
UltrasonicSensor u_center, u_front_right, u_right_front, u_right_back, u_back;

volatile unsigned long bounceTimeP = 0; // variable to hold ms count to debounce a pressed switch
volatile unsigned long bounceTimeL = 0; // variable to hold ms count to debounce a pressed switch
volatile unsigned long bounceTimeO = 0; // variable to hold ms count to debounce a pressed switch

volatile int p = 0;
volatile int l = 0;
volatile int o = 0;

void setup()
{
  odometer.begin();

  u_center.attach(US_FRONT);
  u_front_right.attach(US_FRONT_T);
  u_right_front.attach(US_SIDE_FRONT);
  u_right_back.attach(US_SIDE_BACK);
  u_back.attach(US_BACK);

  pinMode(BUTTON_PARK, INPUT);
  pinMode(BUTTON_LANE, INPUT);
  pinMode(BUTTON_OVERTAKE, INPUT);

  pciSetup(BUTTON_PARK);
  pciSetup(BUTTON_LANE);
  pciSetup(BUTTON_OVERTAKE);

  //	attachInterrupt(BUTTON_PARK, interruptRoutine, RISING);
  //	attachInterrupt(BUTTON_LANE, interruptRoutine, RISING);
  //	attachInterrupt(BUTTON_OVERTAKE, interruptRoutine, RISING);

  Serial.begin(BAUD);
  #ifdef RUN
  waitConnection();

  establishContact('s');
  #endif
}

void loop()
{
  #ifdef RUN
  sendButtonsIDLE();

  encodeAndWrite(ID_IN_ULTRASONIC_CENTER, u_center.getDistance());
  encodeAndWrite(ID_IN_ULTRASONIC_CENTER_R, u_front_right.getDistance());
  encodeAndWrite(ID_IN_ULTRASONIC_SIDE_FRONT, u_right_front.getDistance());
  encodeAndWrite(ID_IN_ULTRASONIC_SIDE_BACK, u_right_back.getDistance());
  encodeAndWrite(ID_IN_ULTRASONIC_BACK , u_back.getDistance());

  encodeAndWrite(ID_IN_ENCODER_L, odometer.getDistance());
  #endif

  #ifdef DEBUG
//  u_center.getDistance();
//  u_front_right.getDistance();
//  u_right_front.getDistance();
//  u_right_back.getDistance();
//  Serial.println(u_back.getDistance());
  Serial.println(odometer.getDistance());
  #endif
}

void encodeAndWrite(int id, int value)
{
  int st = protocol.encode(id, value);

  if (st)
  {
    Serial.write(protocol.getBufferOut(), BUFFER_SIZE); //try this first
  }
}

void establishContact(char toSend)
{
  while (Serial.available() <= 0)
  {
    Serial.println(toSend);   // send a char
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

//void interruptRoutine(void) // handle pin change interrupt for D8 to D13 here
//{
//	if (digitalRead(BUTTON_LANE))
//	{
//		laneInterrupt();
//	}
//	else if (digitalRead(BUTTON_PARK))
//	{
//		parkInterrupt();
//	}
//	else if (digitalRead(BUTTON_OVERTAKE))
//	{
//		overtakeInterrupt();
//	}
//}

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
