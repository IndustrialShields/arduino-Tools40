#include "Modbus.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
ModbusFrame::ModbusFrame(uint8_t slave, uint8_t *pdu) {
	_slave = slave;
	_pdu = pdu;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
ModbusResponse::ModbusResponse(uint8_t slave, uint8_t *pdu) : ModbusFrame(slave, pdu) {

}


////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusResponse::isDiscreteSet(uint16_t offset) const {
	if (_pdu == nullptr) {
		return false;
	}

	// ptr = PDU + FC (1) + BYTE COUNT (1) + OFFSET (offset / 8)
	uint8_t *ptr = _pdu + 2 + (offset >> 3);
	offset &= 0x03; // offset is the bit number
	return (*ptr >> offset) & 0x01;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t ModbusResponse::getRegister(uint16_t offset) const {
	if (_pdu == nullptr) {
		return 0;
	}

	// ptr = PDU + FC(1) + BYTE COUNT (1) + OFFSET (offset * 2)
	uint8_t *ptr = _pdu + 2 + (offset << 1);
	return (uint16_t(*ptr) << 8) + *(ptr + 1); // MSB + LSB
}
