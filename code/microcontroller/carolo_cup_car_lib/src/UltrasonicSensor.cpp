/* Placeholder for an ultrasonic sensor class */
/*
 * @author John Ioannis
 */
#include "CaroloCupCar.h"

const unsigned short UltrasonicSensor::DEFAULT_SRF08_ADDRESS = 112;
const unsigned short UltrasonicSensor::MIN_PING_DELAY = 64;

static unsigned short FIRST_ADDRESS = 112; //please refer to: http://www.robot-electronics.co.uk/htm/srf08tech.html
static unsigned short LAST_ADDRESS = 127;

UltrasonicSensor::UltrasonicSensor()
{
    _address = 0, _delay = 0; //some initial invalid values
}

void UltrasonicSensor::attach(unsigned short address)
{
    Wire.begin();
    _ADDRESS = constrain(address, FIRST_ADDRESS, LAST_ADDRESS); //allow only valid values, between 112 and 127
    _DELAY = MIN_PING_DELAY;
}

/*
 *  The
 *  purpose of providing a limit to the maximum gain is to allow you to fire the sonar faster
 */
void UltrasonicSensor::setGain(unsigned short gainValue)
{
    gainValue = constrain(address, 0, 31);
    Wire.beginTransmission(_ADDRESS); //start transmission
    Wire.write(0x01); //write to GAIN register location 1
    Wire.write(gainValue); //write constrained value
    Wire.endTransmission(); //end transmission
}

/*
 * The range is ((Range Register x 43mm) + 43mm) so setting the Range Register to 0 (0x00) gives a
 * maximum range of 43mm. Setting the Range Register to 1 (0x01) gives a maximum range of 86mm.
 * More usefully, 24 (0x18) gives a range of 1 metre and 140 (0x8C) is 6 metres. Setting 255 (0xFF)
 * gives the original 11 metres (255 x 43 + 43 is 11008mm). (range * 4.3) + 4.3 in centimeters
 */
void UltrasonicSensor::setRange(unsigned short rangeValue)
{
    Wire.beginTransmission(_ADDRESS); //start transmission
    Wire.write(0x02); //write to range register location 2
    Wire.write(rangeValue); //write value
    Wire.endTransmission(); //end transmission
}

void UltrasonicSensor::setPingDelay(unsigned short milliseconds)
{
    if (milliseconds > MIN_PING_DELAY)
    {
        _DELAY = milliseconds;
    }
}

/*
 * setGain(0);
 * setRange(23);
 * command sensor to measure in "inches" (0x50)
 * use 0x51 for centimeters
 * use 0x52 for ping microseconds
 */
unsigned int UltrasonicSensor::getDistance()
{
    //ping
    int reading = 0;
    Wire.beginTransmission(_ADDRESS);
    Wire.write(byte(0x00));
    Wire.write(byte(0x51));
    Wire.endTransmission();

    //read echo
    wait(_DELAY);
    Wire.beginTransmission(_ADDRESS);
    Wire.write(byte(0x02));
    Wire.endTransmission();

    Wire.requestFrom(_ADDRESS, 2);

    if (2 <= Wire.available())
    { // if two bytes were received
        reading = Wire.read();  // receive high byte (overwrites previous reading)
        reading = reading << 8;    // shift high byte to be high 8 bits
        reading |= Wire.read(); // receive low byte as lower 8 bits
    }

    return reading;
}

void UltrasonicSensor::changeAddress(unsigned short newAddress)
{
    newAddress = constrain(newAddress, FIRST_ADDRESS, LAST_ADDRESS);
    Wire.beginTransmission(_ADDRESS);
    Wire.write(byte(0x00));
    Wire.write(byte(0xA0));
    Wire.endTransmission();

    Wire.beginTransmission(_ADDRESS);
    Wire.write(byte(0x00));
    Wire.write(byte(0xAA));
    Wire.endTransmission();

    Wire.beginTransmission(_ADDRESS);
    Wire.write(byte(0x00));
    Wire.write(byte(0xA5));
    Wire.endTransmission();

    Wire.beginTransmission(_ADDRESS);
    Wire.write(byte(0x00));
    Wire.write(newAddress << 1);
    Wire.endTransmission();

    _ADDRESS = newAddress;
}

