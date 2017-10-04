#pragma clang diagnostic push
#pragma ide diagnostic ignored "CannotResolve"
#ifndef CAROLOCUPCAR_H
#define CAROLOCUPCAR_H

#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>

#define BAUD 115200

#define US_FRONT 0x71 //front ultrasonic pin
#define US_FRONT_T 0x72 //front tilted right ultrasonic
#define US_BACK 0x73 //back ultrasonic pin
#define US_SIDE_FRONT 0x74 //side front right ultrasonic
#define US_SIDE_BACK 0x75 //side back ultrasonic pin

#define ENCODER_PIN_A 2 //wheel encoder channel A pin
#define ENCODER_PIN_B 4 //wheel encoder channel B pin

#define SERVO_PIN 3 //steering servo pin
#define ESC_PIN 9 //ESC motor pin

#define CH_1 5 //RC receiver channel 1 pin
#define CH_2 6 //RC receiver channel 2 pin

#define redPin 22 //LED red pin
#define bluePin 26 //LED blue pin
#define INDICATOR_FRONT_LEFT 24 //LED green pin
#define INDICATOR_FRONT_RIGHT 24 //LED green pin
#define INDICATOR_BACK_LEFT 24 //LED green pin
#define INDICATOR_FRONT_RIGHT 24 //LED green pin
#define HEADLIGHTS 24 //LED green pin

//Signal Conditioning limits for RC controller
#define LOW 1000
#define HIGH 2000
#define DEAD_LOW 1430
#define DEAD_HIGH 1570
#define MIDDLE 1500

class DistanceSensor {
public:
    DistanceSensor();
    virtual ~DistanceSensor();
    virtual unsigned int getDistance() = 0;
    unsigned int getMedianDistance(short iterations = DEFAULT_ITERATIONS);
    void encodeAndWrite(int id, int value);
    void wait(long milliseconds);
    unsigned long currentMillis;
    long interval;
private:
    static const short DEFAULT_ITERATIONS;
protected:
    unsigned int _sensorMedianDelay; //delay between measurements in the sensor's getMedianDistance in milliseconds
};

class UltrasonicSensor : public DistanceSensor {//placeholder for a possible future ultrasonic abstract class
public:
    explicit UltrasonicSensor();
    void attach(unsigned short address);
    void setGain(unsigned short gainValue);
    void setRange(unsigned short rangeValue);
    void setPingDelay(unsigned short milliseconds = DEFAULT_PING_DELAY);
    unsigned int getDistance();
    void changeAddress(unsigned short newAddress);
private:
    uint8_t _ADDRESS;
    unsigned short _DELAY;
    static const unsigned short DEFAULT_SRF08_ADDRESS;
    const unsigned short MIN_PING_DELAY;
};

class Odometer {
public:
    explicit Odometer(unsigned long pulsesPerMeter = DEFAULT_PULSES_PER_METER);
    int attach(unsigned short odometerPin);
    int attach(unsigned short odometerPin, unsigned short directionPin, boolean forwardDirState);
    void begin();
    unsigned long getDistance();
    long getRelativeDistance();
    short getDirection();
    float getSpeed();
    void encodeAndWrite(int id, int value);

private:
    unsigned long pulsesToCentimeters(unsigned long pulses);
    int init(unsigned short odometerPin);
    unsigned long _pulsesPerMeter;
    static const unsigned int DEFAULT_PULSES_PER_METER;
    static const unsigned short DEFAULT_DIRECTION_PIN;
    unsigned short _odometerInterruptPin, _odometerID;
    unsigned int _millimetersPerPulse;
};

class SteeringMotor : public Wire {
public:
    explicit SteeringMotor();
    void init(unsigned short pin, uint8_t address);
    void arm();
    void setAngle(int degrees, int isRC);
private:
    uint8_t retrieve(uint8_t _address);
    void send(uint8_t _address, uint8_t val);
    virtual void setDegrees();
    unsigned short _pin; //the pin the Servo motor is attached to
    unsigned int filterAngle(int degrees, int isRC);
    unsigned int _angle;
    unsigned int STRAIGHT_DEGREES; //the values (in degrees) that can get written to the steering motor, with 0 being the leftmost position, 180 the rightmost and 90 around the middle
    unsigned int MAX_RIGHT_ANGLE, MAX_LEFT_ANGLE;
    uint8_t _ADDRESS;
};

class ESCMotor : public Servo {
public:
    explicit ESCMotor();
    void setSpeed(int speed);
    void brake();
    void arm();
    void init(unsigned short pin);
private:
    void setFreqsAndSpeeds();
    unsigned short _pin; //the pin the ESC is attached to
    int filterSpeed(int speed);
    int indicateDirection(int speed);
    int _speed, _direction;
    unsigned short FULL_FORWARD, FULL_BACKWARD, IDLE_SPEED; //what percentage of the motor's power is allowed be used at most
    int IDLE_RAW_SPEED, MAX_FRONT_RAW_SPEED, MAX_BACK_RAW_SPEED; //the raw value (in whatever metric, usually pwm signal) that the motors are idle, throttling full speed backwards and forward
    int red, green, blue;
    void wait(double seconds);
    unsigned long currentMillis;
    long interval;
};

#endif
#pragma clang diagnostic pop