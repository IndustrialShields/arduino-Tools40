/*
   Copyright (c) 2018 Boot&Work Corp., S.L. All rights reserved

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

#include "ModbusSlave.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
ModbusSlave::ModbusSlave(uint8_t addr) {
	setState(Idle);

	_next = nullptr;
	_addr = addr;
	_coils = nullptr;
	_numCoils = 0;
	_discreteInputs = nullptr;
	_numDiscreteInputs = 0;
	_holdingRegisters = nullptr;
	_numHoldingRegisters = 0;
	_inputRegisters = nullptr;
	_numInputRegisters = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t ModbusSlave::processRequest(uint8_t *pdu) {
	uint8_t fc = *pdu++;
	*_next++ = fc;

	switch (fc) {
		case ReadCoils:
			return readCoils(pdu);
		case ReadDiscreteInputs:
			return readDiscreteInputs(pdu);
		case ReadHoldingRegisters:
			return readHoldingRegisters(pdu);
		case ReadInputRegisters:
			return readInputRegisters(pdu);
		case WriteSingleCoil:
			return writeSingleCoil(pdu);
		case WriteSingleRegister:
			return writeSingleRegister(pdu);
		case WriteMultipleCoils:
			return writeMultipleCoils(pdu);
		case WriteMultipleRegisters:
			return writeMultipleRegisters(pdu);
	}

	return IllegalFunction;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t ModbusSlave::readDigitalIO(uint8_t *data, const bool *io, uint16_t numIOs) {
	if (!io) {
		return IllegalFunction;
	}

	uint16_t addr = (data[0] << 8) + data[1];
	uint16_t num = (data[2] << 8) + data[3];

	if (addr + num > numIOs) {
		return IllegalDataAddress;
	}

	// Start response data
	_next = data;

	// Byte count = (num / 8) + (if num % 8 != 0 then 1 else 0)
	uint8_t byteCount = (num >> 3) + ((num & 7) ? 1 : 0);
	*_next = byteCount;

	io += addr; // IO cursor
	uint8_t b = 0; // bit cursor
	for (uint16_t i = 0; i < num; ++i) {
		if (b == 0) {
			*(++_next) = 0;
		}
		if (*io) {
			*_next |= (1 << b);
		}

		b = (b + 1) & 7;
		++io;
	}

	// Add the last byte
	++_next;

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t ModbusSlave::readRegisters(uint8_t *data, const uint16_t *registers, uint16_t numRegisters) {
	if (!registers) {
		return IllegalFunction;
	}

	uint16_t addr = (data[0] << 8) + data[1];
	uint16_t num = (data[2] << 8) + data[3];

	if (addr + num > numRegisters) {
		return IllegalDataAddress;
	}

	// Start response data
	_next = data;

	// Byte count = num * 2
	uint8_t byteCount = num << 1;
	*_next++ = byteCount;

	registers += addr; // Register cursor
	for (uint16_t i = 0; i < num; ++i) {
		*_next++ = *registers >> 8;
		*_next++ = *registers;
		++registers;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t ModbusSlave::writeSingleCoil(uint8_t *data) {
	if (!_coils) {
		return IllegalFunction;
	}

	uint16_t addr = (data[0] << 8) + data[1];
	uint16_t value = (data[2] << 8) + data[3];

	if (addr > _numCoils) {
		return IllegalDataAddress;
	}

	bool *io = _coils + addr;
	if (value == 0xff00) {
		*io = true;
	} else if (value == 0x0000) {
		*io = false;
	}

	// Keep the response equal to the request
	// and send starting address and value
	_next = data + 4;

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t ModbusSlave::writeSingleRegister(uint8_t *data) {
	if (!_holdingRegisters) {
		return IllegalFunction;
	}

	uint16_t addr = (data[0] << 8) + data[1];
	uint16_t value = (data[2] << 8) + data[3];

	if (addr + 1 > _numHoldingRegisters) {
		return IllegalDataAddress;
	}

	*(_holdingRegisters + addr) = value;

	// Keep the response equal to the request
	// and send starting address and value
	_next = data + 4;

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t ModbusSlave::writeMultipleCoils(uint8_t *data) {
	if (!_coils) {
		return IllegalFunction;
	}

	uint16_t addr = (data[0] << 8) + data[1];
	uint16_t num = (data[2] << 8) + data[3];
	uint8_t byteCount = data[4];
	uint8_t *values = data + 5;

	if (addr + num > _numCoils) {
		return IllegalDataAddress;
	}

	bool *ptr = _coils + addr;
	uint8_t b = 0; // Bit cursor
	while (num > 0) {
		*ptr++ = ((*values) >> b) & 0x01;
		b = (b + 1) & 0x07;
		if (b == 0) {
			++values;
		}
		--num;
	}

	// Keep the response equal to the request
	// and send starting address and quantity only
	_next = data + 4;

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t ModbusSlave::writeMultipleRegisters(uint8_t *data) {
	if (!_holdingRegisters) {
		return IllegalFunction;
	}

	uint16_t addr = (data[0] << 8) + data[1];
	uint16_t num = (data[2] << 8) + data[3];
	uint8_t byteCount = data[4];
	uint8_t *values = data + 5;

	if (addr + num > _numHoldingRegisters) {
		return IllegalDataAddress;
	}

	uint16_t *ptr = _holdingRegisters + addr;
	while (num > 0) {
		*ptr++ = (values[0] << 8) + values[1];
		values += 2;
		--num;
	}

	// Keep the response equal to the request
	// and send starting address and quantity only
	_next = data + 4;

	return 0;
}

