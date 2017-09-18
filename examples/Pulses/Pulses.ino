// Pulses library example
// by Industrial Shields

#include <Pulses.h>

// Output pin
// Only pins with an associated timer can be used
// IMPORTANT: when using TIMER0 pins, time related functions (millis, delay, ...)
// lose their time reference and don't work properly
const int pin = 3;

// Frequency in Hz
// Default value: 1000
// NOTE: Use 'unsigned long' and 'UL' suffix for high frequencies
const unsigned long frequency = 500UL; // 500Hz

// Frequency precision
// Default value: 3
// Recommended usage
// precision = 1: from 30Hz to 150Hz
// precision = 2: from 150Hz to 500Hz
// precision = 3: from 500Hz to 4kHz
// precision = 4: from 4kHz to 32kHz
// precision = 5: from 32kHz to 4MHz
const int precision = 4;

////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  // Set pin as OUTPUT
  pinMode(pin, OUTPUT);

  // Start train pulses
  startPulses(pin, frequency, precision);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {

}
