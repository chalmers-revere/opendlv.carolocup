#include "CaroloCupActuators.h"
#include "Protocol.h"

#define T_OUT 5000
Protocol protocol;
SteeringMotor servo;
ESCMotor esc;
RCReceiver receiver;
LEDControl ledControl;

unsigned long currentMillis;
unsigned long interval;

volatile int interrupt = 0;

long oldMillis;
int noData = 0;
int oldNoData = 0;

void setup() {
  Serial.begin(BAUD);
  waitConnection();

  servo.begin();
  esc.init();
  receiver.begin();
  ledControl.begin();

  establishContact('\n');
}

void loop() {
    if(noData && oldNoData != noData){
        servo.setAngle(STRAIGHT_DEGREES);
        esc.brake();
    }

    oldNoData = noData;

    if(receiver.readChannel1()){
       if(!interrupt) {
            esc.brake();
            ledControl.setBrakeLights();
       }
       ledControl.setRCLight();
       interrupt = 1;
       servo.setAngle(receiver.filter(receiver.readChannel1()));
       esc.setSpeed(receiver.filter(receiver.readChannel2()));
    } else {
       if(interrupt) {
            esc.brake();
            ledControl.setBrakeLights();
       }
       timeout();
       interrupt = 0;
    }
}

void establishContact(char toSend) {
  while (Serial.available() <= 0) {
    Serial.println(toSend);   // send a char
    wait(0.5);
  }
  Serial.read();
  wait(5);
  esc.arm();
  ledControl.setIndicators(0, 1); //blink 4 indicators to aware car is on
  ledControl.setHeadLights(1);
}

void waitConnection() {
  while (!Serial); // wait for serial port to connect. Needed for native USB port only
}

void wait(double seconds) {
  interval = seconds * 1000;
  currentMillis = millis();
  while (millis() - currentMillis <= interval);
}

void timeout(){
  oldMillis = millis();
  while (!Serial.available()) {
     if ((millis() - oldMillis) > T_OUT) {
         noData = 1;
         break;
     }
  }
}

void serialEvent() {
  if(!interrupt){
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
            ledControl.setBrakeLights();
            break;
          case ID_OUT_MOTOR:
            esc.setSpeed(value);
            break;
          case ID_OUT_SERVO:
            servo.setAngle(value);
            break;
          case ID_OUT_LIGHTS_EFFECT:
            ledControl.setIndicators(0, 1);
            break;
          case ID_OUT_INDICATOR_LF:
            ledControl.setIndicators(ID_OUT_INDICATOR_LF, 0);
            break;
          case ID_OUT_INDICATOR_RF:
            ledControl.setIndicators(ID_OUT_INDICATOR_RF, 0);
            break;
          case ID_OUT_INDICATOR_LB:
            ledControl.setIndicators(ID_OUT_INDICATOR_LB, 0);
            break;
          case ID_OUT_INDICATOR_RB:
            ledControl.setIndicators(ID_OUT_INDICATOR_RB, 0);
            break;
          default:
            break;
        }
      }
  } else {
    Serial.read();
  }
}

