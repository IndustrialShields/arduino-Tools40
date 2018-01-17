#ifndef __Counter_H__
#define __Counter_H__

#include <Arduino.h>

class Counter {
	public:
		explicit Counter(unsigned long preset);

	public:
		virtual int update(int up, int down, int reset);

	protected:
		unsigned long _value;
		unsigned long _preset;
		int _lastUp;
		int _lastDown;
};

#endif // __Counter_H__
