//
// Created by raphael on 10/30/17.
//
#include "Protocol.h"

Protocol::Protocol()
{
	readCounter = 0;
	header = 0;
	firstTimeHeader = 0;
	valid = 0;
}

void Protocol::setHeader()
{
	bufferOut[0] = HEADER;
}

uint8_t Protocol::checksum()
{
	uint8_t result = 0;
	uint16_t sum = 0;

	for (uint8_t i = 0; i < (BUFFER_SIZE - 1); i++)
	{
		sum += bufferOut[i];
	}
	result = sum & CHECK;

	return result;
}

uint8_t Protocol::encode(int _id, int _value)
{
	if ((_id >= 0 && _id <= 255) && (_value >= 0 && _value <= 504))
	{
		setHeader();

		uint8_t a, b, mod, div;

		mod = _value % 2;
		div = _value / 2;

		a = div + mod;
		b = div;

		bufferOut[1] = _id & 0xFF;
		bufferOut[2] =  a & 0xFF;
		bufferOut[3] =  b & 0xFF;

		bufferOut[4] = checksum();

		return 1;
	}
	else
	{
		return 0;
	}
}

uint8_t *Protocol::getBufferIn()
{
	return bufferIn;
}

uint8_t *Protocol::getBufferOut()
{
	return bufferOut;
}

bool Protocol::isHeader(uint8_t byte)
{
	return byte == HEADER;
}

void Protocol::decode(uint8_t byte)
{
	if (isHeader(byte))
	{

		if (!firstTimeHeader)
		{
			header = 1;
			readCounter = 0;
			firstTimeHeader = 1;
			valid = 0;
		}
	}

	bufferIn[readCounter] = byte;
	readCounter++;

	if (readCounter >= BUFFER_SIZE)
	{
		readCounter = 0;

		if (header)
		{
			uint8_t checksumValue = bufferIn[4];

			if (verifyChecksum(checksumValue))
			{
				data.id = bufferIn[1];
				data.value = bufferIn[2] + bufferIn[3];
				valid = 1;
			}

			header = 0;
			firstTimeHeader = 0;
		}

	}

}

uint8_t Protocol::verifyChecksum(uint8_t obtainedChecksum)
{
	uint8_t result = 0;
	uint16_t sum = 0;

	for (uint8_t i = 0; i < (BUFFER_SIZE - 1); i++)
	{
		sum += bufferIn[i];
	}
	result = sum & CHECK;

	if (obtainedChecksum == result)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

uint8_t Protocol::isValid()
{
	return valid;
}

int Protocol::getId()
{
	return data.id;
}

int Protocol::getValue()
{
	return data.value;
}