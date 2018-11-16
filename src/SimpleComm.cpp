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

#define MAX_DATA_LEN 128
#define BUFFER_SIZE (SYN_LEN + LEN_LEN + HDR_LEN + MAX_DATA_LEN + CRC_LEN)

static uint8_t _txBuffer[BUFFER_SIZE];
static uint8_t _rxBuffer[BUFFER_SIZE];
static uint8_t _rxBufferLen = 0;

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

	if (dlen > MAX_DATA_LEN) {
		return false;
	}

	uint8_t *ptr = _txBuffer;
	*ptr++ = SYN_VALUE;
	*ptr++ = PKT_LEN(dlen);
	*ptr++ = packet.getDestination();
	*ptr++ = packet.getSource();
	*ptr++ = packet.getType();
	if (dlen > 0) {
		memcpy(ptr, data, dlen);
		ptr += dlen;
	}
	*ptr++ = calcCRC(_txBuffer + SYN_LEN + LEN_LEN, HDR_LEN + dlen);

	size_t tlen = ptr - _txBuffer;
	return stream.write(_txBuffer, tlen) == tlen;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool SimpleCommClass::send(Stream &stream, SimplePacket &packet, uint8_t destination, uint8_t type) {
	packet.setType(type);
	return send(stream, packet, destination);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool SimpleCommClass::receive(Stream &stream, SimplePacket &packet) {
	while (stream.available()) {
		uint8_t in = stream.read();

		if ((_rxBufferLen == 0) && (in != SYN_VALUE)) {
			// Unsynchronized
			continue;
		}

		if ((_rxBufferLen == SYN_LEN) && ((in > (HDR_LEN + MAX_DATA_LEN + CRC_LEN)) || (in < (HDR_LEN + CRC_LEN)))) {
			// Invalid data length
			_rxBufferLen = 0;
			continue;
		}

		_rxBuffer[_rxBufferLen++] = in;

		if (_rxBufferLen > SYN_LEN + LEN_LEN + HDR_LEN) {
			uint8_t tlen = _rxBuffer[1];
			if (_rxBufferLen == (tlen + SYN_LEN + LEN_LEN)) {
				// Buffer complete

				// Check CRC
				if (_rxBuffer[SYN_LEN + LEN_LEN + tlen - CRC_LEN] != calcCRC(_rxBuffer + SYN_LEN + LEN_LEN, tlen - CRC_LEN)) {
					// Invalid CRC
					_rxBufferLen = 0;
					continue;
				}

				// Check destination
				// if my address is 0 then receive all messages
				// if destination address is 0 then it is a broadcast message
				if (_address != 0 && _rxBuffer[SYN_LEN + LEN_LEN] != 0 && _rxBuffer[SYN_LEN + LEN_LEN] != _address) {
					// It is not for me
					_rxBufferLen = 0;
					continue;
				}

				uint8_t *ptr = _rxBuffer + SYN_LEN + LEN_LEN;
				packet.setDestination(*ptr++);
				packet.setSource(*ptr++);
				packet.setType(*ptr++);
				if (!packet.setData(ptr, tlen - HDR_LEN - CRC_LEN)) {
					// Internal error
					_rxBufferLen = 0;
					return false;
				}

				_rxBufferLen = 0;
				return true;
			}
		}
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
