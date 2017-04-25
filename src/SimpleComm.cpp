/*
   Copyright (c) 2017 Boot&Work Corp., S.L. All rights reserved

   This library is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "SimpleComm.h"

// PACKET FORMAT:
//  _____________________________________________________________________
// |         |         |                                                 |
// |         |         |                       PKT                       |
// |_________|_________|_________________________________________________|
// |         |         |                             |         |         |
// |   SYN   |   LEN   |            HDR              |   DAT   |   CRC   |
// |_________|_________|_____________________________|_________|_________|
// |         |         |         |         |         |         |         |
// | SYN (1) | LEN (1) | DST (1) | SRC (1) | TYP (1) |   DAT   | CRC (1) |
// |_________|_________|_________|_________|_________|_________|_________|
//

#define SYN_LEN 1
#define LEN_LEN 1
#define DST_LEN 1
#define SRC_LEN 1
#define TYP_LEN 1
#define HDR_LEN (DST_LEN + SRC_LEN + TYP_LEN)
#define CRC_LEN 1

#define PKT_LEN(dlen) (HDR_LEN + (dlen) + CRC_LEN)

#define SYN_VALUE 0x02

static uint8_t _buffer[SYN_LEN + LEN_LEN + 256];

////////////////////////////////////////////////////////////////////////////////////////////////////
SimpleCommClass::SimpleCommClass() {
	_address = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void SimpleCommClass::begin(uint8_t address) {
	_address = address;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool SimpleCommClass::send(Stream &stream, SimplePacket &packet, uint8_t destination) {
	packet.setSource(_address);
	packet.setDestination(destination);

	uint8_t dlen;
	const uint8_t *data = (const uint8_t *) packet.getData(dlen);

	uint8_t *ptr = _buffer;
	*ptr++ = SYN_VALUE;
	*ptr++ = PKT_LEN(dlen);
	*ptr++ = packet.getDestination();
	*ptr++ = packet.getSource();
	*ptr++ = packet.getType();
	if (dlen > 0) {
		memcpy(ptr, data, dlen);
		ptr += dlen;
	}
	*ptr++ = calcCRC(_buffer + SYN_LEN + LEN_LEN, HDR_LEN + dlen);

	size_t tlen = ptr - _buffer;
	return stream.write(_buffer, tlen) == tlen;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool SimpleCommClass::send(Stream &stream, SimplePacket &packet, uint8_t destination, uint8_t type) {
	packet.setType(type);
	return send(stream, packet, destination);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool SimpleCommClass::receive(Stream &stream, SimplePacket &packet) {
	while (stream.available() >= SYN_LEN + LEN_LEN) {
		if (stream.read() != SYN_VALUE) {
			// Unsynchronized
			continue;
		}

		// Get packet length
		uint8_t tlen = stream.read();

		// Get packet
		if (stream.readBytes(_buffer, tlen) != tlen) {
			// Timeout
			continue;
		}

		// Check CRC
		if (_buffer[tlen - CRC_LEN] != calcCRC(_buffer, tlen - CRC_LEN)) {
			// Invalid CRC
			continue;
		}

		// Check destination
		if (_buffer[0] != _address) {
			// It is not for me
			continue;
		}

		uint8_t *ptr = _buffer;
		packet.setDestination(*ptr++);
		packet.setSource(*ptr++);
		packet.setType(*ptr++);
		if (!packet.setData(ptr, tlen - HDR_LEN - CRC_LEN)) {
			// Internal error
			continue;
		}

		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t SimpleCommClass::calcCRC(uint8_t *buffer, size_t len) {
	uint8_t ret = 0;
	while (len--) {
		ret += *buffer++;
	}
	return ret;
}

SimpleCommClass SimpleComm;
