#ifndef __ModbusSlave_H__
#define __ModbusSlave_H__

#include "Modbus.h"

class ModbusSlave : public ModbusDevice {
	protected:
		enum {
			Idle,
			Receiving,
			Sending,
		};

	public:
		explicit ModbusSlave(uint8_t addr);

	public:
		inline uint8_t getSlaveAddress() const {
			return _addr;
		}

		inline void setCoils(bool *coils, uint16_t num) {
			_coils = coils;
			_numCoils = num;
		}

		inline void setDiscreteInputs(bool *discreteInputs, uint16_t num) {
			_discreteInputs = discreteInputs;
			_numDiscreteInputs = num;
		}

		inline void setHoldingRegisters(uint16_t *holdingRegisters, uint16_t num) {
			_holdingRegisters = holdingRegisters;
			_numHoldingRegisters = num;
		}

		inline void setInputRegisters(uint16_t *inputRegisters, uint16_t num) {
			_inputRegisters = inputRegisters;
			_numInputRegisters = num;
		}

	public:
		virtual void update() = 0;

	protected:
		uint8_t processRequest(uint8_t *pdu);

	private:
		uint8_t readDigitalIO(uint8_t *data, const bool *io, uint16_t numIOs);
		uint8_t readRegisters(uint8_t *data, const uint16_t *registers, uint16_t numRegisters);
		uint8_t writeDigitalIO(uint8_t *data, bool *io, uint16_t numIOs);

		uint8_t writeSingleCoil(uint8_t *data);
		uint8_t writeSingleRegister(uint8_t *data);
		uint8_t writeMultipleCoils(uint8_t *data);
		uint8_t writeMultipleRegisters(uint8_t *data);

		inline uint8_t readCoils(uint8_t *data) {
			return readDigitalIO(data, _coils, _numCoils);
		}

		inline uint8_t readDiscreteInputs(uint8_t *data) {
			return readDigitalIO(data, _discreteInputs, _numDiscreteInputs);
		}

		inline uint8_t readHoldingRegisters(uint8_t *data) {
			return readRegisters(data, _holdingRegisters, _numHoldingRegisters);
		}

		inline uint8_t readInputRegisters(uint8_t *data) {
			return readRegisters(data, _inputRegisters, _numInputRegisters);
		}

	protected:
		uint8_t *_next;

	private:
		uint8_t _addr;

		bool *_coils;
		uint16_t _numCoils;
		bool *_discreteInputs;
		uint16_t _numDiscreteInputs;
		uint16_t *_holdingRegisters;
		uint16_t _numHoldingRegisters;
		uint16_t *_inputRegisters;
		uint16_t _numInputRegisters;
};

#endif // __ModbusSlave_H__
