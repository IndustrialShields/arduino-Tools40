#include "ModbusTCPMaster.h"

#if defined(ethernet_h) || defined(ethernet_h_)

////////////////////////////////////////////////////////////////////////////////////////////////////
ModbusTCPMaster::ModbusTCPMaster() {
	_currentTransactionID = 0;
	_currentClient = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusTCPMaster::readCoils(EthernetClient &client, uint8_t slave, uint16_t addr, uint16_t quantity) {
	_currentClient = &client;
	return sendReadRequest(slave, ReadCoils, addr, quantity);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusTCPMaster::readDiscreteInputs(EthernetClient &client, uint8_t slave, uint16_t addr, uint16_t quantity) {
	_currentClient = &client;
	return sendReadRequest(slave, ReadDiscreteInputs, addr, quantity);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusTCPMaster::readHoldingRegisters(EthernetClient &client, uint8_t slave, uint16_t addr, uint16_t quantity) {
	_currentClient = &client;
	return sendReadRequest(slave, ReadHoldingRegisters, addr, quantity);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusTCPMaster::readInputRegisters(EthernetClient &client, uint8_t slave, uint16_t addr, uint16_t quantity) {
	_currentClient = &client;
	return sendReadRequest(slave, ReadInputRegisters, addr, quantity);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusTCPMaster::writeSingleCoil(EthernetClient &client, uint8_t slave, uint16_t addr, bool value) {
	_currentClient = &client;
	return sendWriteSingleRequest(slave, WriteSingleCoil, addr, value ? 0xFF00 : 0x0000);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusTCPMaster::writeSingleRegister(EthernetClient &client, uint8_t slave, uint16_t addr, uint16_t value) {
	_currentClient = &client;
	return sendWriteSingleRequest(slave, WriteSingleRegister, addr, value);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusTCPMaster::writeMultipleCoils(EthernetClient &client, uint8_t slave, uint16_t addr, const bool *values, uint16_t quantity) {
	_currentClient = &client;
	return sendWriteMultipleCoilsRequest(slave, addr, values, quantity);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusTCPMaster::writeMultipleRegisters(EthernetClient &client, uint8_t slave, uint16_t addr, const uint16_t *values, uint16_t quantity) {
	_currentClient = &client;
	return sendWriteMultipleRegistersRequest(slave, addr, values, quantity);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusTCPMaster::prepareRequest(uint8_t slave, uint8_t fc) {
	if (getState() != Idle) {
		// Invalid state
		return false;
	}

	_next = _adu;
	++_currentTransactionID;
	_currentSlave = slave;
	_currentFC = fc;

	// Header
	*_next++ = _currentTransactionID >> 8; // Transaction ID Hi
	*_next++ = _currentTransactionID; // Transaction ID Lo
	*_next++ = 0x00; // Protocol ID Hi
	*_next++ = 0x00; // Protocol ID Lo
	*_next++ = 0x00; // Length Hi
	*_next++ = 0x00; // Length Lo
	*_next++ = slave; // Unit ID
	*_next++ = fc; // Function code

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusTCPMaster::sendRequest() {
	if (_currentClient == nullptr) {
		// No current client
		return false;
	}

	if (!_currentClient->connected()) {
		// Client not connected
		return false;
	}

	// Set length
	uint16_t len = _next - _adu - 6;
	_adu[4] = len >> 8; // Length Hi
	_adu[5] = len; // Length Lo

	// Ignore pending data
	while (_currentClient->available()) {
		_currentClient->read();
	}

	// Send the packet
	bool ret = _currentClient->write(_adu, _next - _adu) == uint16_t(_next - _adu);
	if (ret) {
		_lastRequestTime = millis();
		setState(WaitingResponse);
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
ModbusResponse ModbusTCPMaster::available() {
	uint8_t *responsePDU = nullptr;

	if (_currentClient) {
		if (getState() == Receiving || getState() == WaitingResponse) {
			if (_currentClient->available()) {
				if (getState() == WaitingResponse) {
					setState(Receiving);
					_next = _adu;
				}

				uint16_t responseLen = _next - _adu;

				do {
					if (responseLen >= MODBUS_TCP_ADU_SIZE) {
						// Overflow error
						setState(Idle);
						// TODO notify to user
						break;
					}

					// Receive next byte
					*_next++ = _currentClient->read();
					++responseLen;

					if (responseLen >= MODBUS_TCP_HEADER_SIZE) {
						// Packet length is available
						uint16_t packetLen = (_adu[4] << 8) + _adu[5];
						if (packetLen == responseLen - 6) {
							// Packet complete

							// Check errors
							uint16_t transactionID = (_adu[0] << 8) + _adu[1];
							if (transactionID != _currentTransactionID) {
								// Invalid transaction ID
								// TODO notify to user
							} else if (_adu[2] != 0x00 && _adu[3] != 0x00) {
								// Invalid protocol ID
								// TODO notify to user
							} else if (_adu[6] != _currentSlave) {
								// Bad slave error
								// TODO notify to user
							} else if ((_adu[7] & 0x7f) != _currentFC) {
								// Bad function code
								// TODO notify to user
							} else {
								// TODO Check data length

								// Valid response
								responsePDU = _adu + 7;
							}

							setState(Idle);
							break;
						}
					}
				} while (_currentClient->available());
			} else if (millis() - _lastRequestTime > MODBUS_TCP_RESPONSE_TIMEOUT) {
				// Response timeout error
				setState(Idle);
				// TODO notify to user
			}
		}
	}

	if (responsePDU != nullptr) {
		_currentClient = nullptr;
	}

	return ModbusResponse(_currentSlave, responsePDU);
}

#endif
