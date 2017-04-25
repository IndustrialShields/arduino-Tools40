#ifndef __Filter_H__
#define __Filter_H__

#include <Arduino.h>

// N: number of samples
// T: sample period
template<unsigned N, unsigned T>
class Filter {
	public:
		explicit Filter() {
			memset(_samples, 0, sizeof(int) * N);
			_nextSample = 0;
			_lastSampleTime = 0;
		}

	public:
		virtual int update(int value) = 0;

	protected:
		void sample(int value) {
			if (millis() - _lastSampleTime >= T) {
				_samples[_nextSample++] = value;
				if (_nextSample >= N) {
					_nextSample = 0;
				}
			}
		}

	protected:
		int _samples[N];

	private:
		unsigned _nextSample;
		unsigned long _lastSampleTime;
};

template<unsigned N, unsigned T>
class AnalogFilter : public Filter<N, T> {
	public:
		virtual int update(int value) {
			AnalogFilter::sample(value);

			int ret = 0;
			int *ptr = AnalogFilter::_samples;

			for (unsigned i = 0; i < N; ++i) {
				ret += *ptr++;
			}

			return ret / N;
		}
};

#endif // __Filter_H__
