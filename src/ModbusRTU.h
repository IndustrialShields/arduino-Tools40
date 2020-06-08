#ifndef __ModbusRTU_H__
#define __ModbusRTU_H__

#include "Modbus.h"

#define MODBUS_RTU_ADU_SIZE				(3 + MODBUS_PDU_SIZE) // ADU = SLAVE (1) + PDU + CRC (2)
#define MODBUS_RTU_RESPONSE_TIMEOUT		1000 //ms
#define MODBUS_RTU_HEADER_SIZE			(1) // SLAVE (1)

// T3.5 = 3.5 * 10E6 us_per_s * 11 bits_per_char / rate bits_per_second
#define MODBUS_RTU_T35US(rate) (((rate) > 19200UL) ? 1750UL : (11UL * 3500000UL / (rate)))

// T1.5 = 1.5 * 10E6 us_per_s * 11 bits_per_char / rate bits_per_second
#define MODBUS_RTU_T15US(rate) (((rate) > 19200UL) ? 750UL : (11UL * 1500000UL / (rate)))

#endif // __ModbusRTU_H__
