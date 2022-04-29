#include "Modbus.h"

const char STR_NO_EXCEPTION[] = "";
const char STR_TIMEOUT_EXCEPTION[] = "timeout";
const char STR_OVERFLOW_EXCEPTION[] = "overflow";
const char STR_INVALID_TRANSACTION_ID_EXCEPTION[] = "transaction id";
const char STR_INVALID_PROTOCOL_ID_EXCEPTION[] = "protocol id";
const char STR_BAD_SLAVE_ERROR_EXCEPTION[] = "bad slave error";
const char STR_BAD_FUNCTION_CODE_EXCEPTION[] = "bad function code";
const char STR_BAD_DATA_LENGTH_EXCEPTION[] = "bad data length";
const char STR_CRC_EXCEPTION[] = "invalid crc";
const char STR_UNKNOWN_EXCEPTION[] = "unknown exception";

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
	offset &= 0x07; // offset is the bit number
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
