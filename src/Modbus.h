#ifndef __Modbus_H__
#define __Modbus_H__

#include <Arduino.h>

#define MODBUS_DATA_SIZE				(252) // bytes
#define MODBUS_PDU_SIZE 				(1 + MODBUS_DATA_SIZE) // PDU = FC (1) + DATA
#define MODBUS_RESPONSE_TIMEOUT			1000

class ModbusDevice {
	public:
		enum FunctionCodes {
			ReadCoils				= 1,
			ReadDiscreteInputs		= 2,
			ReadHoldingRegisters	= 3,
			ReadInputRegisters		= 4,
			WriteSingleCoil			= 5,
			WriteSingleRegister		= 6,
			WriteMultipleCoils		= 15,
			WriteMultipleRegisters	= 16,
		};

		enum Errors {
			IllegalFunction			= 0x01,
			IllegalDataAddress		= 0x02,
			IllegalDataValue		= 0x03,
			ServerDeviceFailure		= 0x04,
		};

		inline void setTimeout(uint32_t timeout) {
			_timeout = timeout;
		}

	protected:
		inline uint8_t getState() const {
			return _state;
		}

		inline void setState(uint8_t state) {
			_state = state;
		}
		uint32_t _timeout = MODBUS_RESPONSE_TIMEOUT;

	private:
		uint8_t _state;
};

class ModbusFrame {
	public:
		explicit ModbusFrame(uint8_t slave, uint8_t *pdu = nullptr);

	public:
		inline bool isNull() const {
			return _pdu == nullptr;
		}

	public:
		inline operator bool() const {
			return !isNull();
		}

	protected:
		uint8_t *_pdu;
		uint8_t _slave;
};

class ModbusResponse : public ModbusFrame {
	public:
		explicit ModbusResponse(uint8_t slave, uint8_t *pdu = nullptr);

	public:
		inline bool hasError() const {
			return *_pdu & 0x80;
		}

		inline uint8_t getErrorCode() const {
			return _pdu[1];
		}

		inline uint8_t getSlave() const {
			return _slave;
		}

		inline uint8_t getFC() const {
			return *_pdu & 0x7f;
		}

		inline bool isCoilSet(uint16_t offset) const {
			return isDiscreteSet(offset);
		}

		inline bool isDiscreteInputSet(uint16_t offset) const {
			return isDiscreteSet(offset);
		}

		bool isDiscreteSet(uint16_t offset) const;
		uint16_t getRegister(uint16_t offset) const;
};

#endif // __Modbus_H__
