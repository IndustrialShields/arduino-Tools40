#include "ModbusRTUMaster.h"

#include "utilities/crc16.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
ModbusRTUResponse::ModbusRTUResponse(uint8_t slave, uint8_t *pdu) : ModbusResponse(pdu) {
	_slave = slave;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
ModbusRTUMaster::ModbusRTUMaster(HardwareSerial &serial) : _serial(serial) {
	_state = Idle;
	_t35us = 0UL;
	_t15us = 0UL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ModbusRTUMaster::begin(uint32_t rate, int mode) {
	_serial.begin(rate, mode);

	if (rate > 19200UL) {
		// Recommended times for high speeds
		_t35us = 1750UL; // us
		_t15us = 750UL; // us
	} else {
		// T3.5 = 3.5 * 10E6 us_per_s * 11 bits_per_char / rate bits_per_second
		_t35us = 11UL * 3500000UL / rate;

		// T1.5 = 1.5 * 10E6 us_per_s * 11 bits_per_char / rate bits_per_second
		_t15us = 11UL * 1500000UL / rate;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusRTUMaster::prepareRequest(uint8_t slave, uint8_t fc) {
	if (_state != Idle) {
		// Invalid state
		return false;
	}

	if (slave > 247) {
		// Invalid slave
		return false;
	}

	_aduPtr = _adu;
	_currentSlave = slave;
	_currentFC = fc;

	// SLAVE
	*_aduPtr++ = slave;

	// FC
	*_aduPtr++ = fc;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusRTUMaster::sendRequest() {
	// CRC
	uint16_t crc = crc16(_adu, _aduPtr - _adu);
	*_aduPtr++ = crc >> 8;
	*_aduPtr++ = crc;

	_state = PreSending;
	_last35Time = micros();

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusRTUMaster::sendReadRequest(uint8_t slave, uint8_t fc, uint16_t addr, uint16_t quantity) {
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
	*_aduPtr++ = addr >> 8;
	*_aduPtr++ = addr;
	*_aduPtr++ = quantity >> 8;
	*_aduPtr++ = quantity;

	// START SEND PROCESS
	return sendRequest();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusRTUMaster::sendWriteSingleRequest(uint8_t slave, uint8_t fc, uint16_t addr, uint16_t value) {
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
	*_aduPtr++ = addr >> 8;
	*_aduPtr++ = addr;
	*_aduPtr++ = value >> 8;
	*_aduPtr++ = value;

	// START SEND PROCESS
	return sendRequest();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusRTUMaster::writeMultipleCoils(uint8_t slave, uint16_t addr, const bool *values, uint16_t quantity) {
	if (quantity < 1 || quantity > 0x07b0) {
		// Invalid quantity
		return false;
	}

	if (!prepareRequest(slave, WriteMultipleCoils)) {
		// Request prepare error
		return false;
	}

	// DATA
	*_aduPtr++ = addr >> 8;
	*_aduPtr++ = addr;
	*_aduPtr++ = quantity >> 8;
	*_aduPtr++ = quantity;
	*_aduPtr++ = ((quantity - 1) >> 3) + 1;

	uint8_t bit = 0;
	const bool *ptr = values;
	*_aduPtr = 0;
	while (quantity > 0) {
		*_aduPtr |= (*ptr++ ? 1 : 0) << bit;
		if (bit == 7) {
			bit = 0;
			*(++_aduPtr) = 0;
		} else {
			++bit;
		}
		--quantity;
	}
	if (bit > 0) {
		++_aduPtr;
	}

	// START SEND PROCESS
	return sendRequest();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusRTUMaster::writeMultipleRegisters(uint8_t slave, uint16_t addr, const uint16_t *values, uint16_t quantity) {
	if (quantity < 1 || quantity > 0x007b) {
		// Invalid quantity
		return false;
	}

	if (!prepareRequest(slave, WriteMultipleRegisters)) {
		// Request prepare error
		return false;
	}

	// DATA
	*_aduPtr++ = addr >> 8;
	*_aduPtr++ = addr;
	*_aduPtr++ = quantity >> 8;
	*_aduPtr++ = quantity;
	*_aduPtr++ = quantity << 1;

	const uint16_t *ptr = values;
	while (quantity-- > 0) {
		*_aduPtr++ = *ptr >> 8;
		*_aduPtr++ = *ptr++;
	}

	// START SEND PROCESS
	return sendRequest();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
ModbusRTUResponse ModbusRTUMaster::available() {
	uint8_t *responsePdu = nullptr;

	if (_state == PreSending) {
		if (micros() - _last35Time > _t35us) {
			// Flush received bytes
			while (_serial.available()) {
				_serial.read();
			}

			// Start sending
			_state = Sending;
			_serial.write(_adu, _aduPtr - _adu);
		}
	}

	if (_state == Sending) {
		if (_serial.availableForWrite() >= SERIAL_TX_BUFFER_SIZE - 1) {
			// Transmission finished -> start T3.5
			_state = PostSending;
			_last35Time = micros();
		}
	}

	if (_state == PostSending) {
		if (micros() - _last35Time > _t35us) {
			// T3.5 expired -> start waiting response
			_state = WaitingResponse;
			_lastRequestTime = millis();
		}
	}

	if (_state == WaitingResponse) {
		if (_serial.available()) {
			// Data is available -> start receiving
			_state = Receiving;
			_aduPtr = _adu;
			_last15Time = micros();
		} else if (millis() - _lastRequestTime > MODBUS_RTU_RESPONSE_TIMEOUT) {
			// Response timeout error
			_state = Idle;
			// TODO notify to user
		}
	}

	if (_state == Receiving) {
		if (_serial.available()) {
			do {
				// Check ADU buffer used length
				if (_aduPtr - _adu >= MODBUS_RTU_ADU_SIZE) {
					// Overflow error
					_state = Idle;
					// TODO notify to user
					break;
				}
				*_aduPtr++ = _serial.read();

				_last15Time = micros();
			} while (_serial.available());
		} else if (micros() - _last15Time > _t15us) {
			// Response is finished

			// Response length
			uint16_t responseLen = _aduPtr - _adu;

			// Check errors
			if (responseLen < 3) {
				// Bad length error
			} else {
				// Calculate CRC of the response
				uint16_t crc = crc16(_adu, responseLen - 2);
				uint16_t responseCRC = (uint16_t(*(_aduPtr - 2)) << 8) | (*(_aduPtr - 1));
				if (crc != responseCRC) {
					// Invalid CRC error
					// TODO notify to user
				} else if (_adu[0] != _currentSlave) {
					// Bad slave error
					// TODO notify to user
				} else if (_adu[1] != _currentFC) {
					// Bad function code
					// TODO notify to user
				} else {
					// TODO Check data length

					// Valid response
					responsePdu = _adu + 1;
				}
			}

			_state = Idle;
		}
	}

	return ModbusRTUResponse(_currentSlave, responsePdu);
}
