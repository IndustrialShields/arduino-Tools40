
#ifndef __Modbus_ModbusRTUSlave_H__
#define __Modbus_ModbusRTUSlave_H__

#include "ModbusRTU.h"
#include "ModbusSlave.h"

class ModbusRTUSlave : public ModbusSlave {
	public:
		ModbusRTUSlave(HardwareSerial &serial, uint8_t addr);

	public:
		void begin(uint32_t rate = 19200UL);
		virtual void update();

	private:
		bool sendResponse();

	private:
		// Configuration properties
		HardwareSerial &_serial;
#if defined(ESP32)
		uint16_t _tx_buffer_size;
#endif
		uint32_t _t35us;
		uint32_t _t15us;

		// State properties
		uint32_t _last35Time;
		uint32_t _last15Time;
		uint8_t _adu[MODBUS_RTU_ADU_SIZE];
};

#endif
