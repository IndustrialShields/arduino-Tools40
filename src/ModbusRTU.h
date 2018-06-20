#ifndef __ModbusRTU_H__
#define __ModbusRTU_H__

#include "Modbus.h"

#define MODBUS_RTU_ADU_SIZE				(3 + MODBUS_PDU_SIZE) // ADU = SLAVE (1) + PDU + CRC (2)
#define MODBUS_RTU_RESPONSE_TIMEOUT		1000 //ms

#endif // __ModbusRTU_H__
