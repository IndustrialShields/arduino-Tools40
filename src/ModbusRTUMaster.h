#ifndef __Modbus_ModbusRTUMaster_H__
#define __Modbus_ModbusRTUMaster_H__

#include "ModbusRTU.h"
#include "ModbusMaster.h"

class ModbusRTUMaster : public ModbusMaster {
	public:
		explicit ModbusRTUMaster(HardwareSerial &serial);

	public:
		void begin(uint32_t rate = 19200UL);

		inline bool readCoils(uint8_t slave, uint16_t addr, uint16_t quantity) {
			return sendReadRequest(slave, ReadCoils, addr, quantity);
		}
		inline bool readDiscreteInputs(uint8_t slave, uint16_t addr, uint16_t quantity) {
			return sendReadRequest(slave, ReadDiscreteInputs, addr, quantity);
		}
		inline bool readHoldingRegisters(uint8_t slave, uint16_t addr, uint16_t quantity) {
			return sendReadRequest(slave, ReadHoldingRegisters, addr, quantity);
		}
		inline bool readInputRegisters(uint8_t slave, uint16_t addr, uint16_t quantity) {
			return sendReadRequest(slave, ReadInputRegisters, addr, quantity);
		}
		inline bool writeSingleCoil(uint8_t slave, uint16_t addr, bool value) {
			return sendWriteSingleRequest(slave, WriteSingleCoil, addr, value ? 0xFF00 : 0x0000);
		}
		inline bool writeSingleRegister(uint8_t slave, uint16_t addr, uint16_t value) {
			return sendWriteSingleRequest(slave, WriteSingleRegister, addr, value);
		}
		bool writeMultipleCoils(uint8_t slave, uint16_t addr, const bool *values, uint16_t quantity) {
			return sendWriteMultipleCoilsRequest(slave, addr, values, quantity);
		}
		bool writeMultipleRegisters(uint8_t slave, uint16_t addr, const uint16_t *values, uint16_t quantity) {
			return sendWriteMultipleRegistersRequest(slave, addr, values, quantity);
		}

		virtual ModbusResponse available();

	protected:
		virtual bool prepareRequest(uint8_t slave, uint8_t fc);
		virtual bool sendRequest();

	private:
		// Configuration properties
		HardwareSerial &_serial;
		uint32_t _t35us;
		uint32_t _t15us;

		// State properties
		uint32_t _last35Time;
		uint32_t _last15Time;
		uint32_t _lastRequestTime;
		uint8_t _currentSlave;
		uint8_t _currentFC;

		uint8_t _adu[MODBUS_RTU_ADU_SIZE];
};

#endif
