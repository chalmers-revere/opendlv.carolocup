#include "CaroloCupSensors.h"
#include "Protocol.h"

#define BOUNCE_DURATION 500   // define an appropriate bounce time in ms for your switches

unsigned long currentMillis;
unsigned long interval;

Protocol protocol;
Axes axes;
Odometer odometer;
UltrasonicSensor u_center, u_front_right, u_right_front, u_right_back, u_back;

volatile unsigned long bounceTimeP = 0; // variable to hold ms count to debounce a pressed switch
volatile unsigned long bounceTimeL = 0; // variable to hold ms count to debounce a pressed switch
volatile unsigned long bounceTimeO = 0; // variable to hold ms count to debounce a pressed switch

void setup() {
    Serial.begin(BAUD);
    waitConnection();
    axes.begin();

    //odometer.begin();

    u_center.attach(US_FRONT);
    u_front_right.attach(US_FRONT_T);
    u_right_front.attach(US_SIDE_FRONT);
    u_right_back.attach(US_SIDE_BACK);
    u_back.attach(US_BACK);

    //establishContact('\n');

    pinMode(BUTTON_PARK, INPUT);
    pinMode(BUTTON_LANE, INPUT);
    pinMode(BUTTON_OVERTAKE, INPUT);

    //attachInterrupt(digitalPinToInterrupt(BUTTON_PARK), parkInterrupt, CHANGE);
    //attachInterrupt(digitalPinToInterrupt(BUTTON_LANE), laneInterrupt, CHANGE);
    //attachInterrupt(digitalPinToInterrupt(BUTTON_OVERTAKE), overtakeInterrupt, CHANGE);
}

void loop() {
    sendButtonsIDLE();

    //encodeAndWrite(ID_IN_ULTRASONIC_CENTER, u_center.getDistance());
    //encodeAndWrite(ID_IN_ULTRASONIC_CENTER_R, u_front_right.getDistance());
    //encodeAndWrite(ID_IN_ULTRASONIC_SIDE_FRONT, u_right_front.getDistance());
    //encodeAndWrite(ID_IN_ULTRASONIC_SIDE_BACK, u_right_back.getDistance());
    //encodeAndWrite(ID_IN_ULTRASONIC_BACK , u_back.getDistance());

    //encodeAndWrite(ID_IN_ENCODER_L, odometer.getDistance());

    axes.readMotion();
    //encodeAndWrite(ID_IN_AX, axes.getAX());
    //encodeAndWrite(ID_IN_AY, axes.getAY());
    //encodeAndWrite(ID_IN_AZ, axes.getAZ());

    //encodeAndWrite(ID_IN_GX, axes.getGX());
    //encodeAndWrite(ID_IN_GY, axes.getGY());
    //encodeAndWrite(ID_IN_GZ, axes.getGZ());

    //encodeAndWrite(ID_IN_STEP, axes.getStep());

    Serial.println(axes.getGZ());

    Serial.println(axes.getGX());

    Serial.println(axes.getGY());
}

void encodeAndWrite(int id, int value)
{
	int st = protocol.encode(id, value);

	if (st)
	{
		//for (int i = 0; i < BUFFER_SIZE; i++)
		//{ protocol.getBufferOut()[i]
            Serial.write(protocol.getBufferOut(), BUFFER_SIZE); //try this first
		//}
	}
}

void establishContact(char toSend) {
  while (Serial.available() <= 0) {
    Serial.println(toSend);   // send a char
    wait(0.5);
  }
  Serial.read();
}

void waitConnection() {
  while (!Serial); // wait for serial port to connect. Needed for native USB port only
}

void wait(double seconds) {
  interval = seconds * 1000;
  currentMillis = millis();
  while (millis() - currentMillis <= interval);
}

void sendButtonsIDLE() {
    encodeAndWrite(ID_IN_BUTTON_PARK, 2);
    encodeAndWrite(ID_IN_BUTTON_LANE, 2);
    encodeAndWrite(ID_IN_BUTTON_OVERTAKE, 2);
}

void parkInterrupt() {
  if (abs(millis() - bounceTimeP) > BOUNCE_DURATION)
  {
    encodeAndWrite(ID_IN_BUTTON_PARK, digitalRead(BUTTON_PARK));

    bounceTimeP = millis();  // set whatever bounce time in ms is appropriate
  }
}

void laneInterrupt() {
  if (abs(millis() - bounceTimeL) > BOUNCE_DURATION)
  {
    encodeAndWrite(ID_IN_BUTTON_LANE, digitalRead(BUTTON_LANE));

    bounceTimeL = millis();  // set whatever bounce time in ms is appropriate
  }
}

void overtakeInterrupt() {
  if (abs(millis() - bounceTimeO) > BOUNCE_DURATION)
  {
    encodeAndWrite(ID_IN_BUTTON_OVERTAKE, digitalRead(BUTTON_OVERTAKE));

    bounceTimeO = millis();  // set whatever bounce time in ms is appropriate
  }
}
