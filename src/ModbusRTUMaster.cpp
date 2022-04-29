#include "ModbusRTUMaster.h"

#include "utilities/crc16.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
ModbusRTUMaster::ModbusRTUMaster(HardwareSerial &serial) : _serial(serial) {
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ModbusRTUMaster::begin(uint32_t rate) {
	_t35us = MODBUS_RTU_T35US(rate);
	_t15us = MODBUS_RTU_T15US(rate);
	_timeout = MODBUS_RTU_RESPONSE_TIMEOUT;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusRTUMaster::prepareRequest(uint8_t slave, uint8_t fc) {
	if (!isIdle()) {
		// Invalid state
		return false;
	}

	if (slave == 0 || slave > 247) {
		// Invalid slave
		return false;
	}

	_currentSlave = slave;
	_currentFC = fc;

	_next = _adu;
	*_next++ = _currentSlave; // Slave
	*_next++ = _currentFC; // Function code

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusRTUMaster::sendRequest() {
	// CRC
	uint16_t crc = crc16(_adu, _next - _adu);
	*_next++ = crc >> 8;
	*_next++ = crc;

	setState(PreSending);
	_last35Time = micros();

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
ModbusResponse ModbusRTUMaster::available() {
	uint8_t *responsePDU = nullptr;

	if (getState() == PreSending) {
		if (micros() - _last35Time > _t35us) {
			// Flush received bytes
			while (_serial.available()) {
				_serial.read();
			}

			// Start sending
			setState(Sending);
			_serial.write(_adu, _next - _adu);
		}
	}

	if (getState() == Sending) {
#if defined(SERIAL_TX_BUFFER_SIZE)
		if (_serial.availableForWrite() >= SERIAL_TX_BUFFER_SIZE - 1) {
#else
		if (_serial.availableForWrite() >= 0x7f - 1) {
#endif
			// Transmission finished -> start T3.5
			setState(PostSending);
			_last35Time = micros();
		}
	}

	if (getState() == PostSending) {
		if (micros() - _last35Time > _t35us) {
			// T3.5 expired -> start waiting response
			setState(WaitingResponse);
			_lastRequestTime = millis();
		}
	}

	if (getState() == WaitingResponse) {
		if (_serial.available()) {
			// Discard first 0x00 byte
			if (_serial.peek() != 0x00) {
				// Data is available -> start receiving
				setState(Receiving);
				_next = _adu;
				_last15Time = micros();
			} else {
				// Discard first zero
				_serial.read();
			}
		} else if (millis() - _lastRequestTime > _timeout) {
			// Response timeout error
			setState(Idle);

			setException(TimeoutException);
		}
	}

	if (getState() == Receiving) {
		if (_serial.available()) {
			do {
				// Check ADU buffer used length
				if (_next - _adu >= MODBUS_RTU_ADU_SIZE) {
					// Overflow error
					setState(Idle);
					setException(OverflowException);
					break;
				}
				*_next++ = _serial.read();

				_last15Time = micros();
			} while (_serial.available());
		} else if (micros() - _last15Time > _t15us) {
			// Response is finished

			// Response length
			uint16_t responseLen = _next - _adu;

			// Check errors
			if (responseLen < 3) {
				// Bad length error
				setException(BadDataLengthException);
			} else {
				// Calculate CRC of the response
				uint16_t crc = crc16(_adu, responseLen - 2);
				uint16_t responseCRC = (uint16_t(*(_next - 2)) << 8) | (*(_next - 1));
				if (crc != responseCRC) {
					// Invalid CRC error
					setException(BadCRCException);
				} else if (_adu[0] != _currentSlave) {
					// Bad slave error
					setException(BadSlaveErrorException);
				} else if (_adu[1] != _currentFC) {
					// Bad function code
					setException(BadFunctionCodeException);
				} else {
					// TODO Check data length

					// Valid response
					responsePDU = _adu + 1;
				}
			}

			setState(Idle);
		}
	}

	return ModbusResponse(_currentSlave, responsePDU);
}
