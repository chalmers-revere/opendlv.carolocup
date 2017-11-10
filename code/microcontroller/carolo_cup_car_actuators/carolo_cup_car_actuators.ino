#include "CaroloCupActuators.h"
#include "Protocol.h"

//#define DEBUG
#define RUN

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

void setup() {
    axes.begin();
    servo.init();
    esc.init();
    receiver.begin();
    ledControl.begin();

    ledControl.setIndicators(ID_OUT_LIGHTS_EFFECT, 0.5); //blink all leds to aware car is on

    attachInterrupt(digitalPinToInterrupt(CH_1), interruptRoutine, RISING);

    Serial.begin(BAUD);

#ifdef RUN
    waitConnection();

    //establishContact('a');
#endif
}

void loop() {
    _blink++;
    if (_blink > 2147483647) _blink = 0;

    if (noData && (oldNoData != noData) && !interrupt) {
        servo.setAngle(STRAIGHT_DEGREES);
        esc.brake();
        esc.arm();
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
        ledControl.setRCLight(45, _blink);

        if (!interrupt) {
            esc.brake();
            esc.arm();
            ledControl.setBrakeLights(_ON_);
            wait(2);
        }

        interrupt = 1;

        int angle = receiver.readChannel1();
        int speed = receiver.readChannel2();

        if (angle == 0) {

            esc.brake();
            ledControl.setBrakeLights(_ON_);

            interrupt = 0;
            rcControllerFlag = 0;

            attachInterrupt(digitalPinToInterrupt(CH_1), interruptRoutine, RISING);
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

void establishContact(char toSend) {
    while (Serial.available() <= 0) {
        Serial.println(toSend);   // send a char
        wait(0.5);
    }
    Serial.read();
    wait(5);
    esc.arm();
    ledControl.setIndicators(LED_SIGNAL, 0.5); //blink all leds to aware car is on
    //ledControl.setHeadLights(_ON_);
}

void waitConnection() {
    while (!Serial); // wait for serial port to connect. Needed for native USB port only
}

void wait(double seconds) {
    interval = seconds * 1000000;
    currentMillis = micros();
    while (micros() - currentMillis <= interval);
}

void timeout() {
    oldMillis = micros();
    while (!Serial.available()) {

        if ((micros() - oldMillis) > T_OUT) {
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
                    ledControl.setIndicators(value, 0.5);
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
    int a = pulseMeasure(CH_1);

#ifdef DEBUG
    Serial.print("interrupt ch1 ");
    Serial.println(a);
#endif

    if (a >= DEAD_LOW && a <= DEAD_HIGH) {
        rcControllerFlag++;
    } else {
        if (interrupt) {
            esc.brake();
            esc.arm();
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
        esc.arm();
        ledControl.setBrakeLights(_ON_);
        detachInterrupt(digitalPinToInterrupt(CH_1));
    }
}

unsigned long pulseMeasure(uint8_t pin) {

    uint8_t state = HIGH;
    unsigned long pulseWidth = 0;
    unsigned long loopCount = 0;
    unsigned long loopMax = 5000000;

    // While the pin is *not* in the target state we make sure the timeout hasn't been reached.
    while ((digitalRead(pin)) != state) {
        if (loopCount++ == loopMax) {
            return 0;
        }
    }
    // When the pin *is* in the target state we bump the counter while still keeping track of the timeout.
    while ((digitalRead(pin)) == state) {
        if (loopCount++ == loopMax) {
            return 0;
        }
        pulseWidth++;
    }
    // Return the pulse time in microsecond!
    return pulseWidth * 1.45; // Calculated the pulseWidth++ loop to be about 1.50uS in length.
}


