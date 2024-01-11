#ifndef __Modbus_H__
#define __Modbus_H__

#include <Arduino.h>

#define MODBUS_DATA_SIZE				(252) // bytes
#define MODBUS_PDU_SIZE 				(1 + MODBUS_DATA_SIZE) // PDU = FC (1) + DATA
#define MODBUS_RESPONSE_TIMEOUT			1000

extern const char STR_NO_EXCEPTION[];
extern const char STR_TIMEOUT_EXCEPTION[];
extern const char STR_OVERFLOW_EXCEPTION[];
extern const char STR_INVALID_TRANSACTION_ID_EXCEPTION[];
extern const char STR_INVALID_PROTOCOL_ID_EXCEPTION[];
extern const char STR_BAD_SLAVE_ERROR_EXCEPTION[];
extern const char STR_BAD_FUNCTION_CODE_EXCEPTION[];
extern const char STR_BAD_DATA_LENGTH_EXCEPTION[];
extern const char STR_CRC_EXCEPTION[];
extern const char STR_UNKNOWN_EXCEPTION[];

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

		enum Exceptions {
			NoException = 0,
			TimeoutException,
			OverflowException,
			InvalidTransactionIdException,
			InvalidProtocolIdException,
			BadSlaveErrorException,
			BadFunctionCodeException,
			BadDataLengthException,
			BadCRCException
		};

		const char* getExceptionMessage() {
			return getExceptionMessage(_exception);
		}

		const char* getExceptionMessage(enum Exceptions e) {
			switch(e) {
				case NoException:
					return STR_NO_EXCEPTION;
				break;
				case TimeoutException:
					return STR_TIMEOUT_EXCEPTION;
				break;
				case OverflowException:
					return STR_OVERFLOW_EXCEPTION;
				break;
				case InvalidTransactionIdException:
					return STR_INVALID_TRANSACTION_ID_EXCEPTION;
				break;
				case InvalidProtocolIdException:
					return STR_INVALID_PROTOCOL_ID_EXCEPTION;
				break;
				case BadSlaveErrorException:
					return STR_BAD_SLAVE_ERROR_EXCEPTION;
				break;
				case BadFunctionCodeException:
					return STR_BAD_FUNCTION_CODE_EXCEPTION;
				break;
				case BadDataLengthException:
					return STR_BAD_DATA_LENGTH_EXCEPTION;
				break;
				case BadCRCException:
					return STR_CRC_EXCEPTION;
				break;
			}

			return STR_UNKNOWN_EXCEPTION;
		}

		inline void setTimeout(uint32_t timeout) {
			_timeout = timeout;
		}

		inline bool hasException() {
			return _exception != NoException;
		}

		inline enum Exceptions getException() {
			return _exception;
		}

		inline void clearException() {
			_exception = NoException;
		}

	protected:
		inline void setException(enum Exceptions e) {
			_exception = e;
		}

		inline uint8_t getState() const {
			return _state;
		}

		inline void setState(uint8_t state) {
			_state = state;
		}
		uint32_t _timeout = MODBUS_RESPONSE_TIMEOUT;

	private:
		uint8_t _state;
		enum Exceptions _exception = NoException;
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
