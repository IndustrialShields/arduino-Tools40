#ifndef __Counter_H__
#define __Counter_H__

#include <Arduino.h>

class Counter {
	public:
		explicit Counter(uint32_t preset);

	public:
		virtual int update(int up, int down, int reset);

	protected:
		uint32_t _value;
		uint32_t _preset;
		int _lastUp;
		int _lastDown;
};

#endif // __Counter_H__
