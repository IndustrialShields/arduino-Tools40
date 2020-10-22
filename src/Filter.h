#ifndef __Filter_H__
#define __Filter_H__

#include <Arduino.h>

class Filter {
	public:
		virtual int update(int value) = 0;
};

// N: number of samples
// T: sample period
template<size_t N, uint32_t T>
class AnalogFilter : public Filter {
	public:
		explicit AnalogFilter(int initialValue = 0) {
			for (size_t i = 0; i < N; ++i) {
				_samples[i] = initialValue;
			}
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
			int32_t ret = 0;
			int16_t *ptr = _samples;
			for (size_t i = 0; i < N; ++i) {
				ret += *ptr++;
			}

			return ret / N;
		}

	private:
		uint32_t _lastSampleTime;
		int16_t _samples[N];
		size_t _nextSample;
};

// T: filtering time
template<uint32_t T>
class DigitalFilter : public Filter {
	public:
		DigitalFilter(int initialValue = LOW) {
			_lastTime = 0UL;
			_lastValue = initialValue;
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
		uint32_t _lastTime;
		int _lastValue;
};

#endif // __Filter_H__
