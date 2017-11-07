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

long oldMillis;
int noData = 0;
int oldNoData = 0;

void setup()
{
  servo.begin();
  esc.init();
  receiver.begin();
  ledControl.begin();
  pciSetup(CH_1); // set interrupt for emergency brake

  Serial.begin(BAUD);
  waitConnection();

  //establishContact('a');
}

void loop()
{
  if (noData && (oldNoData != noData) && !interrupt)
  {
    servo.setAngle(STRAIGHT_DEGREES);
    esc.brake();
    ledControl.setBrakeLights(1);
    Serial.println("OUT");
  }

  oldNoData = noData;

  timeout();
}

void establishContact(char toSend)
{
  while (Serial.available() <= 0)
  {
    Serial.println(toSend);   // send a char
    wait(0.5);
  }
  Serial.read();
  wait(5);
  esc.arm();
  ledControl.setIndicators(ID_OUT_LIGHTS_EFFECT, 500); //blink 4 indicators to aware car is on
  ledControl.setHeadLights(_ON_);
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

void timeout()
{
  oldMillis = millis();
  while (!Serial.available())
  {

    if ((millis() - oldMillis) > T_OUT)
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
    uint8_t incoming = Serial.read();
    noData = 0;
    protocol.decode(incoming);
    if (protocol.isValid())
    {
      int value = protocol.getValue();
      switch (protocol.getId())
      {
        case ID_OUT_BRAKE:
          Serial.println("B");
          esc.brake();
          ledControl.setBrakeLights(_ON_);
          break;
        case ID_OUT_MOTOR:
          Serial.println("M");
          ledControl.setBrakeLights(_OFF_);
          esc.setSpeed(value);
          break;
        case ID_OUT_SERVO:
          Serial.println("S");
          servo.setAngle(value);
          break;
        case ID_OUT_INDICATORS:
          Serial.println("I");
          ledControl.setIndicators(value, 500);
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

void pciSetup(byte pin)
{
  *digitalPinToPCMSK(pin) |= bit(digitalPinToPCMSKbit(pin));  // enable pin
  PCIFR |= bit(digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
  PCICR |= bit(digitalPinToPCICRbit(pin)); // enable interrupt for the group
}

ISR(PCINT1_vect) // handle pin change interrupt for A0 to A5 here
{
  _blink++;
  if (_blink > 2147483647) _blink = 0;

  int a = receiver.readChannel1();

  if (a >= DEAD_LOW && a <= DEAD_HIGH)
  {
    rcControllerFlag++;
  }
  else if (a == 0)
  {
    if (interrupt)
    {
      esc.brake();
      ledControl.setBrakeLights(_ON_);
    }
    interrupt = 0;
    rcControllerFlag = 0;
  }

  if (rcControllerFlag >= 3)
  {
    ledControl.setRCLight(20, _blink); //blink rc blue light 1Hz

    if (!interrupt)
    {
      esc.brake();
      ledControl.setBrakeLights(_ON_);
    }
    interrupt = 1;

    ledControl.setBrakeLights(_OFF_);
    //servo.setAngle(receiver.filter(receiver.readChannel1()));
    //esc.setSpeed(receiver.filter(receiver.readChannel2()));

    Serial.print("steer ");
    Serial.println(receiver.filter(receiver.readChannel1()));
    Serial.print("speed ");
    Serial.println(receiver.filter(receiver.readChannel2()));
  }

}
