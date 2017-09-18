#ifndef __PULSES_H__
#define __PULSES_H__

#include <Arduino.h>

// startPulses
// Start a train of pulses in the Arduino pin at freq Hz
// The precision must be a value between 1 and 5
void startPulses(int pin, unsigned long freq = 1000, char precision = 3);

// stopPulses
// Stop the train of pulses running in the Arduino pin pin
void stopPulses(int pin);

#endif // __PULSES_H__
