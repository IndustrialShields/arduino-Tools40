#ifndef __ModbusMaster_H__
#define __ModbusMaster_H__

#include "Modbus.h"

class ModbusMaster : public ModbusDevice {
	protected:
		enum States {
			Idle,
			PreSending,
			Sending,
			PostSending,
			WaitingResponse,
			Receiving,
			WaitingTurnaround,
		};

	public:
		explicit ModbusMaster();

	public:
		inline bool isIdle() const {
			return _state == Idle;
		}
		inline bool isWaitingResponse() const {
			return !isIdle();
		}

		virtual ModbusResponse available() = 0;

	protected:
		inline uint8_t getState() const {
			return _state;
		}

		inline void setState(uint8_t state) {
			_state = state;
		}

		bool sendReadRequest(uint8_t slave, uint8_t fc, uint16_t addr, uint16_t quantity);
		bool sendWriteSingleRequest(uint8_t slave, uint8_t fc, uint16_t addr, uint16_t value);
		bool sendWriteMultipleCoilsRequest(uint8_t slave, uint16_t addr, const bool *values, uint16_t quantity);
		bool sendWriteMultipleRegistersRequest(uint8_t slave, uint16_t addr, const uint16_t *values, uint16_t quantity);

		virtual bool prepareRequest(uint8_t slave, uint8_t fc) = 0;
		virtual bool sendRequest() = 0;

	protected:
		uint8_t *_next;

	private:
		uint8_t _state;
};

#endif
