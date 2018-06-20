#include "crc16.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t crc16(const uint8_t *ptr, uint16_t len) {
	int i;
	uint16_t crc = 0xffff;

	while (len--) {
		crc ^= *ptr++;
		for (i = 0; i < 8; ++i) {
			crc = (crc & 1) ? ((crc >> 1) ^ 0xA001) : (crc >> 1);
		}
	}

	return ((crc >> 8) & 0xff) | ((crc << 8) & 0xff00);
}
