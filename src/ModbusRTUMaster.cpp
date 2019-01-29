#include "ModbusRTUMaster.h"

#include "utilities/crc16.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
ModbusRTUMaster::ModbusRTUMaster(HardwareSerial &serial) : _serial(serial) {
	_t35us = 0UL;
	_t15us = 0UL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ModbusRTUMaster::begin(uint32_t rate) {
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
		if (_serial.availableForWrite() >= SERIAL_TX_BUFFER_SIZE - 1) {
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
			// Data is available -> start receiving
			setState(Receiving);
			_next = _adu;
			_last15Time = micros();
		} else if (millis() - _lastRequestTime > MODBUS_RTU_RESPONSE_TIMEOUT) {
			// Response timeout error
			setState(Idle);
			// TODO notify to user: timeout
		}
	}

	if (getState() == Receiving) {
		if (_serial.available()) {
			do {
				// Check ADU buffer used length
				if (_next - _adu >= MODBUS_RTU_ADU_SIZE) {
					// Overflow error
					setState(Idle);
					// TODO notify to user
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
				// TODO notify to user
			} else {
				// Calculate CRC of the response
				uint16_t crc = crc16(_adu, responseLen - 2);
				uint16_t responseCRC = (uint16_t(*(_next - 2)) << 8) | (*(_next - 1));
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
					responsePDU = _adu + 1;
				}
			}

			setState(Idle);
		}
	}

	return ModbusResponse(_currentSlave, responsePDU);
}
