#ifndef __Modbus_ModbusTCPMaster_H__
#define __Modbus_ModbusTCPMaster_H__

#include "ModbusTCP.h"
#include "ModbusMaster.h"

#if defined(ethernet_h)

class ModbusTCPMaster : public ModbusMaster {
	public:
		explicit ModbusTCPMaster();

	public:
		bool readCoils(EthernetClient &client, uint8_t slave, uint16_t addr, uint16_t quantity);
		bool readDiscreteInputs(EthernetClient &client, uint8_t slave, uint16_t addr, uint16_t quantity);
		bool readHoldingRegisters(EthernetClient &client, uint8_t slave, uint16_t addr, uint16_t quantity);
		bool readInputRegisters(EthernetClient &client, uint8_t slave, uint16_t addr, uint16_t quantity);
		bool writeSingleCoil(EthernetClient &client, uint8_t slave, uint16_t addr, bool value);
		bool writeSingleRegister(EthernetClient &client, uint8_t slave, uint16_t addr, uint16_t value);
		bool writeMultipleCoils(EthernetClient &client, uint8_t slave, uint16_t addr, const bool *values, uint16_t quantity);
		bool writeMultipleRegisters(EthernetClient &client, uint8_t slave, uint16_t addr, const uint16_t *values, uint16_t quantity);

		virtual ModbusResponse available();

	protected:
		virtual bool prepareRequest(uint8_t slave, uint8_t fc);
		virtual bool sendRequest();

	private:
		// State properties
		uint32_t _lastRequestTime;
		uint16_t _currentTransactionID;
		uint8_t _currentSlave;
		uint8_t _currentFC;
		EthernetClient *_currentClient;

		uint8_t _adu[MODBUS_TCP_ADU_SIZE];
};

#endif

#endif
