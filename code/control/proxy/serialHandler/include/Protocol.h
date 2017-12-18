//
// Created by raphael on 10/30/17.
//

#ifndef PROTOCOL_LIBRARY_H
#define PROTOCOL_LIBRARY_H

#include <stdint.h>
#include <iostream>

#define BUFFER_SIZE 5
#define HEADER 0x7E
#define CHECK 0xFF

#define ID_LIGHTS               0
#define ID_MOTOR                1
#define ID_SERVO                2

#define FORWARD_START           100
#define BACKWARD_START          60

/**
 * Final data form
 */
typedef struct {
	int id;
	int value;
	int sub_id;
} protocol_data;

class Protocol
{
public:
	explicit Protocol();

	uint8_t *getBufferIn();

	uint8_t *getBufferOut();

	uint8_t encode(int _id, int _value);

	uint8_t encodeOneByte(int _id, int sub_id, int _value);

	void decode(uint8_t byte);

	uint8_t decodeOneByte(uint8_t byte);

	uint8_t isValid();

	int getId();

	int getSubId();

	int getValue();

	protocol_data getData();

private:
	uint8_t bufferOut[BUFFER_SIZE];

	uint8_t bufferIn[BUFFER_SIZE];

	uint8_t readCounter;
	uint8_t header;

	//Flag that helps us restart counter when we first find header byte
	uint8_t firstTimeHeader;

	uint8_t valid;

	void setHeader();

	uint8_t checksum();

	uint8_t verifyChecksum(uint8_t originalResult);

	bool isHeader(uint8_t byte);

	protocol_data data;

};

#endif //CAROLOCUP_SOFTWARE_PROTOCOL_H
