#include "ModbusRTUSlave.h"
#include "utilities/crc16.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
ModbusRTUSlave::ModbusRTUSlave(HardwareSerial &serial, uint8_t addr) : ModbusSlave(addr), _serial(serial) {

}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ModbusRTUSlave::begin(uint32_t rate) {
	_t35us = MODBUS_RTU_T35US(rate);
	_t15us = MODBUS_RTU_T15US(rate);
#if defined(ESP32)
	_serial.flush();
	_tx_buffer_size = _serial.availableForWrite();
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ModbusRTUSlave::update() {
	if (getState() == Idle) {
		if (_serial.available()) {
			_next = _adu;
			setState(Receiving);
		}
	}

	if (getState() == Receiving) {
		if (_serial.available()) {
			do {
				// Check ADU buffer used length
				if (_next - _adu >= MODBUS_RTU_ADU_SIZE) {
					// Overflow error
					setState(Idle);
#ifdef DEBUG
					Serial.println("----- OVERFLOW -----");
#endif
					// TODO notify to user
					break;
				}
				*_next++ = _serial.read();

				_last15Time = micros();
			} while (_serial.available());

		} else if (micros() - _last15Time > _t15us) {
			// Request is finished

			// Request length
			uint16_t requestLen = _next - _adu;

			// Check errors
			if (requestLen < 3) {
				// Bad length error
				// TODO notify to user
#if DEBUG
				Serial.println("Modbus bad length");
#endif
				setState(Idle);
			} else {
				// Calculate CRC of the request
				uint16_t crc = crc16(_adu, requestLen - 2);
				uint16_t requestCRC = (uint16_t(*(_next - 2)) << 8) | (*(_next - 1));
				if (crc != requestCRC) {
					// Invalid CRC error
					// TODO notify to user
#if DEBUG
					Serial.println("Modbus invalid CRC");
#endif
					setState(Idle);

				} else if (_adu[0] != _addr) {
					// Request not for me
					setState(Idle);

				} else {
					// TODO Check data length
					// Valid request
					uint8_t err = processRequest(_adu + MODBUS_RTU_HEADER_SIZE);
					if (err) {
						_next = _adu + MODBUS_RTU_HEADER_SIZE;
						*_next++ |= 0x80;
						*_next++ = err;
					}
					_last35Time = micros();
					setState(PreSending);
				}
			}
		}
	}

	if (getState() == PreSending) {
		if (micros() - _last35Time > _t35us) {
			// T3.5 expired -> start sending response
			sendResponse();
			setState(Sending);
		}
	}

	if (getState() == Sending) {
#if defined(AVR)
	#if defined(SERIAL_TX_BUFFER_SIZE)
		if (_serial.availableForWrite() >= SERIAL_TX_BUFFER_SIZE - 1) {
	#else
		if (_serial.availableForWrite() >= 0x7f  - 1) {
	#endif // SERIAL_TX_BUFFER_SIZE
#endif // AVR
#if defined(ESP32)
		if (_serial.availableForWrite() >= _tx_buffer_size - 1) {
#endif // ESP32
			// Transmission finished
			setState(Idle);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusRTUSlave::sendResponse() {
	// Set length
	uint16_t crc = crc16(_adu, _next - _adu);
	*_next++ = crc >> 8;
	*_next++ = crc;

	// Send the packet
	uint16_t tlen = _next - _adu;
	return _serial.write(_adu, tlen) == tlen;
}

