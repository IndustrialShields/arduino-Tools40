#include "ModbusTCPSlave.h"

#if defined(ethernet_h)

////////////////////////////////////////////////////////////////////////////////////////////////////
ModbusTCPSlave::ModbusTCPSlave(uint16_t port) : ModbusSlave(0), _server(port) {
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ModbusTCPSlave::begin() {
	_next = nullptr;
	setState(Idle);
	_server.begin();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ModbusTCPSlave::update() {
	if (getState() == Idle) {
		_currentClient = _server.available();
		if (_currentClient) {
			_next = _adu;
			setState(Receiving);
		}
	}

	if (getState() == Receiving) {
		if (receiveRequest()) {
			uint8_t err = processRequest(_adu + MODBUS_TCP_HEADER_SIZE);
			if (err) {
				_next = _adu + MODBUS_TCP_HEADER_SIZE;
				*_next++ |= 0x80;
				*_next++ = err;
			}

			sendResponse();
			setState(Idle);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusTCPSlave::receiveRequest() {
	if (_currentClient.available()) {
		uint16_t requestLen = _next - _adu;

		do {
			if (requestLen >= MODBUS_TCP_ADU_SIZE) {
				// Overflow error
				setState(Idle);
				break;
			}

			// Receive next byte
			*_next++ = _currentClient.read();
			++requestLen;

			if (requestLen >= MODBUS_TCP_HEADER_SIZE) {
				// Packet length is available
				uint16_t packetLen = (_adu[4] << 8) + _adu[5];
				if (packetLen == requestLen - 6) {
					// Packet complete

					// Check errors
					if (_adu[2] != 0x00 && _adu[3] != 0x00) {
						// Invalid protocol ID
						setState(Idle);
						break;
					}
					// TODO Check data length

					// Valid response
					return true;
				}
			}
		} while (_currentClient.available());

		_lastRequestTime = millis();
	} else if (millis() - _lastRequestTime > MODBUS_TCP_RESPONSE_TIMEOUT) {
		// Response timeout error
		setState(Idle);
		return false;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusTCPSlave::sendResponse() {
	if (!_currentClient.connected()) {
		// Client not connected
		return false;
	}

	// Set length
	uint16_t tlen = _next - _adu;
	uint16_t len = tlen - 6;
	_adu[4] = len >> 8; // Length Hi
	_adu[5] = len; // Length Lo

	// Send the packet
	return _currentClient.write(_adu, tlen) == tlen;
}

#endif
