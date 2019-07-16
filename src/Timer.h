#ifndef __Timer_H__
#define __Timer_H__

#include <Arduino.h>

class Timer {
	public:
		explicit Timer(uint32_t presetTime);

	public:
		virtual int update(int in) = 0;

	protected:
		uint32_t _presetTime;
		uint32_t _lastTime;
		int _lastIn;
};

class PulseTimer : public Timer {
	public:
		explicit PulseTimer(uint32_t presetTime);

	public:
		virtual int update(int in);

	private:
		bool _running;
};

// A: active level (HIGH or LOW)
// B: inactive level (HIGH or LOW)
template<int A, int B>
class DelayTimer : public Timer {
	public:
		DelayTimer(uint32_t presetTime) : Timer(presetTime) {
			_lastIn = B;
		}

	public:
		virtual int update(int in) {
			int ret = B;

			if (in == A) {
				if (_lastIn == A) {
					if (millis() - _lastTime > _presetTime) {
						ret = A;
					}
				} else {
					_lastTime = millis();
				}
			}

			_lastIn = in;

			return ret;
		}
};

typedef DelayTimer<HIGH, LOW> OnDelayTimer;
typedef DelayTimer<LOW, HIGH> OffDelayTimer;

#endif // __Timer_H__
