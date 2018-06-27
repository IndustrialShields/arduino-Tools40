#include "ModbusMaster.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
ModbusMaster::ModbusMaster() {
	_state = Idle;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusMaster::sendReadRequest(uint8_t slave, uint8_t fc, uint16_t addr, uint16_t quantity) {
	switch (fc) {
		case ReadCoils:
		case ReadDiscreteInputs:
			if (quantity < 1 || quantity > 2000) {
				// Invalid quantity
				return false;
			}
			break;

		case ReadHoldingRegisters:
		case ReadInputRegisters:
			if (quantity < 1 || quantity > 125) {
				// Invalid quantity
				return false;
			}
			break;

		default:
			// Invalid FC
			return false;
	}

	if (!prepareRequest(slave, fc)) {
		// Request prepare error
		return false;
	}

	// DATA
	*_next++ = addr >> 8;
	*_next++ = addr;
	*_next++ = quantity >> 8;
	*_next++ = quantity;

	// START SEND PROCESS
	return sendRequest();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusMaster::sendWriteSingleRequest(uint8_t slave, uint8_t fc, uint16_t addr, uint16_t value) {
	switch (fc) {
		case WriteSingleCoil:
		case WriteSingleRegister:
			break;

		default:
			// Invalid FC
			return false;
	}

	if (!prepareRequest(slave, fc)) {
		// Request prepare error
		return false;
	}

	// DATA
	*_next++ = addr >> 8;
	*_next++ = addr;
	*_next++ = value >> 8;
	*_next++ = value;

	// START SEND PROCESS
	return sendRequest();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusMaster::sendWriteMultipleCoilsRequest(uint8_t slave, uint16_t addr, const bool *values, uint16_t quantity) {
	if (quantity < 1 || quantity > 0x07b0) {
		// Invalid quantity
		return false;
	}

	if (!prepareRequest(slave, WriteMultipleCoils)) {
		// Request prepare error
		return false;
	}

	// DATA
	*_next++ = addr >> 8;
	*_next++ = addr;
	*_next++ = quantity >> 8;
	*_next++ = quantity;
	*_next++ = ((quantity - 1) >> 3) + 1;

	uint8_t bit = 0;
	const bool *ptr = values;
	*_next = 0;
	while (quantity > 0) {
		*_next |= (*ptr++ ? 1 : 0) << bit;
		if (bit == 7) {
			bit = 0;
			*(++_next) = 0;
		} else {
			++bit;
		}
		--quantity;
	}
	if (bit > 0) {
		++_next;
	}

	// START SEND PROCESS
	return sendRequest();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusMaster::sendWriteMultipleRegistersRequest(uint8_t slave, uint16_t addr, const uint16_t *values, uint16_t quantity) {
	if (quantity < 1 || quantity > 0x007b) {
		// Invalid quantity
		return false;
	}

	if (!prepareRequest(slave, WriteMultipleRegisters)) {
		// Request prepare error
		return false;
	}

	// DATA
	*_next++ = addr >> 8;
	*_next++ = addr;
	*_next++ = quantity >> 8;
	*_next++ = quantity;
	*_next++ = quantity << 1;

	const uint16_t *ptr = values;
	while (quantity-- > 0) {
		*_next++ = *ptr >> 8;
		*_next++ = *ptr++;
	}

	// START SEND PROCESS
	return sendRequest();
}
