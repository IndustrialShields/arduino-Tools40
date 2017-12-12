#ifndef __Filter_H__
#define __Filter_H__

#include <Arduino.h>

class Filter {
	public:
		virtual int update(int value) = 0;
};

// N: number of samples
// T: sample period
template<unsigned N, unsigned long T>
class AnalogFilter : public Filter {
	public:
		explicit AnalogFilter() {
			memset(_samples, 0, sizeof(int) * N);
			_nextSample = 0;
			_lastSampleTime = 0;
		}

	public:
		virtual int update(int value) {
			sample(value);
			return mean();
		}

	private:
		void sample(int value) {
			if (millis() - _lastSampleTime >= T) {
				_samples[_nextSample++] = value;
				if (_nextSample >= N) {
					_nextSample = 0;
				}
			}
		}

		int mean() {
			int ret = 0;
			int *ptr = _samples;
			for (unsigned i = 0; i < N; ++i) {
				ret += *ptr++;
			}

			return ret / N;
		}

	private:
		unsigned long _lastSampleTime;
		int _samples[N];
		unsigned _nextSample;
};

// T: filtering time
template<unsigned long T>
class DigitalFilter : public Filter {
	public:
		DigitalFilter() {
			_lastTime = 0UL;
			_lastValue = LOW;
		}

	public:
		virtual int update(int value) {
			if (value != _lastValue) {
				if (millis() - _lastTime >= T) {
					_lastTime = millis();
					_lastValue = value;
				}
			} else {
				_lastTime = millis();
			}
			return _lastValue;
		}

	private:
		unsigned long _lastTime;
		int _lastValue;
};

#endif // __Filter_H__
