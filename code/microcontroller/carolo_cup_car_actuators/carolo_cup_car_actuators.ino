#include "CaroloCupActuators.h"
#include "Protocol.h"

#define DEBUG
//#define RUN

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

long oldMillis;
int noData = 0;
int oldNoData = 0;

void setup() {
    axes.begin();
    servo.init();
    esc.init();
    receiver.begin();
    ledControl.begin();

    //attachInterrupt(digitalPinToInterrupt(CH_1), interruptRoutine, RISING);
    pciSetup(CH_1);
    Serial.begin(BAUD);
    ledControl.setIndicators(LED_SIGNAL, 300); //blink all leds to aware car is on

#ifdef RUN
    waitConnection();

    establishContact('a');
#endif
}

void loop() {
    _blink++;
    if (_blink > 2147483647) _blink = 0;

    if (noData && (oldNoData != noData) && !interrupt) {
        servo.setAngle(STRAIGHT_DEGREES);
        esc.brake();
        ledControl.setBrakeLights(_ON_);
#ifdef DEBUG
        Serial.println("OUT");
#endif
    }

    oldNoData = noData;

#ifdef RUN
    if (!interrupt) timeout();
#endif

    if (rcControllerFlag >= 3) {
        int angle = receiver.readChannel1();
        int speed = receiver.readChannel2();
        ledControl.setRCLight(45, _blink);

        if (angle == 0) {
            if (interrupt) {
                esc.brake();
                ledControl.setBrakeLights(_ON_);
            }
            interrupt = 0;
            rcControllerFlag = 0;

            //attachInterrupt(digitalPinToInterrupt(CH_1), interruptRoutine, RISING);
        }

        if (!interrupt) {
            esc.brake();
            ledControl.setBrakeLights(_ON_);
        }
        interrupt = 1;

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

    axes.readMotion();
#ifdef DEBUG
    //Serial.print("YAW ");
    //Serial.println(axes.getYaw());
    //Serial.println(receiver.readChannel1());
    //Serial.println(receiver.readChannel2());
#endif
#ifdef RUN
    encodeAndWrite(ID_IN_YAW, axes.getYaw());
#endif
}

void establishContact(char toSend) {
    while (Serial.available() <= 0) {
        Serial.println(toSend);   // send a char
        wait(0.5);
    }
    Serial.read();
    wait(5);
    esc.arm();
    ledControl.setIndicators(LED_SIGNAL, 300); //blink all leds to aware car is on
    //ledControl.setHeadLights(_ON_);
}

void waitConnection() {
    while (!Serial); // wait for serial port to connect. Needed for native USB port only
}

void wait(double seconds) {
    interval = seconds * 1000;
    currentMillis = millis();
    while (millis() - currentMillis <= interval);
}

void timeout() {
    oldMillis = millis();
    while (!Serial.available()) {

        if ((millis() - oldMillis) > T_OUT) {
            noData = 1;
            break;
        }
    }
}

void encodeAndWrite(int id, int value) {
    int st = protocol.encode(id, value);

    if (st) {
        Serial.write(protocol.getBufferOut(), BUFFER_SIZE); //try this first
    }
}

void serialEvent() {
    if (!interrupt) {
        uint8_t incoming = Serial.read();
        noData = 0;
        protocol.decode(incoming);
        if (protocol.isValid()) {
            int value = protocol.getValue();
            switch (protocol.getId()) {
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
                    ledControl.setIndicators(value, 300);
                    break;
                default:
                    break;
            }
        }
    } else {
        Serial.read();
    }
}

void interruptRoutine() {
    int a = receiver.readChannel1();

#ifdef DEBUG
    Serial.print("interrupt ch1 ");
    Serial.println(a);
    Serial.println(micros());
#endif

    if (a >= DEAD_LOW && a <= DEAD_HIGH) {
        rcControllerFlag++;
    } else if (a == 0) {
        if (interrupt) {
            esc.brake();
            ledControl.setBrakeLights(_ON_);
        }
        interrupt = 0;
        rcControllerFlag = 0;
    }

    if (rcControllerFlag >= 3) {
#ifdef DEBUG
        Serial.println("ISR");
#endif
        esc.brake();
        ledControl.setBrakeLights(_ON_);
        //detachInterrupt(digitalPinToInterrupt(CH_1));
    }
}

void pciSetup(byte pin)
{
    *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
    PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
    PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
}

ISR (PCINT2_vect) // handle pin change interrupt for D0 to D7 here
        {
                interruptRoutine();
        }

