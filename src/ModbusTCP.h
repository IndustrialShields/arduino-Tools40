#ifndef __ModbusTCP_H__
#define __ModbusTCP_H__

#if defined(MDUINO) || defined(MDUINO_PLUS)

#include <Ethernet.h>

#include "Modbus.h"

#define MODBUS_TCP_HEADER_SIZE			(7) // TRANSACTION ID (2) + PROTOCOL ID (2) + LENGTH (2) + UNIT ID (1)
#define MODBUS_TCP_ADU_SIZE				(MODBUS_TCP_HEADER_SIZE + MODBUS_PDU_SIZE) // ADU = HEADER + PDU
#define MODBUS_TCP_RESPONSE_TIMEOUT		1000 //ms

#endif

#endif // __ModbusTCP_H__
