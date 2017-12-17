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
		bufferOut[2] = a & 0xFF;
		bufferOut[3] = b & 0xFF;

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

int Protocol::getSubId()
{
	return data.sub_id;
}

int Protocol::getValue()
{
	return data.value;
}

protocol_data Protocol::getData()
{
	return data;
}

uint8_t Protocol::encodeOneByte(int _id, int sub_id, int _value)
{
	uint8_t tosend = 0;
	tosend = (_id & 0X03) << 6;

	if (data.id == ID_LIGHTS)
	{
		tosend |= (sub_id << 1);
		tosend |= _value;
	}
	else if (data.id == ID_SERVO)
	{
		tosend |= (_value / 3);
	}
	else if (data.id == ID_MOTOR)
	{
		if (_value >= -31 && _value <= 31)
		{
			if (_value < 0)
			{
				tosend |= 0X20; //negative sets bit 5 to one
				tosend |= _value;
			}
			else
			{
				tosend |= _value;
			}
		}
	}

	return tosend;
}

uint8_t Protocol::decodeOneByte(uint8_t byte)
{
	uint8_t received = 0;
	received = (byte & 0XC0) >> 6;
	data.id = received;

	if (data.id == ID_LIGHTS)
	{
		uint8_t sub_id = 0;
		sub_id = (byte & 0X3E) >> 1;
		data.sub_id = sub_id;

		received = 0;
		received = byte & 0X01;
		data.value = received;

		return data.id;
	}
	else if (data.id == ID_SERVO)
	{
		uint8_t _received = 0;
		_received = byte & 0X3F;
		data.value = _received * 3;

		return data.id;
	}
	else if (data.id == ID_MOTOR)
	{
		uint8_t _received_ = 0;
		uint8_t v_dec = 0;

		_received_ = byte & 0X20;
		if (!_received_) // positive
		{
			v_dec = byte & 0X1F;
			data.value = FORWARD_START + v_dec;
			if (v_dec == 31) data.value = 90;
		}
		else
		{
			v_dec = byte & 0X1F;
			data.value = BACKWARD_START - v_dec;
			if (v_dec == 31) data.value = 90;
		}

		return data.id;
	}
}