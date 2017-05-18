/*
   Copyright (c) 2017 Boot&Work Corp., S.L. All rights reserved

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Filter.h>

// Create a filter
// Number of samples: 10
// Sample period: 5ms (setting it to 0, samples are taken as fast as possible)
AnalogFilter<10, 5> filter;

#if defined(ARDBOX_ANALOG)
int pin = I0_1;
#elif defined(ARDBOX_RELAY)
int pin = I0_4;
#elif defined(MDUINO_19R) || defined(MDUINO_38R) || defined(MDUINO_57R)
int pin = I0_2;
#elif defined(MDUINO_21) || defined(MDUINO_42) || defined(MDUINO_58)
int pin = I0_7;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  // Init Serial port
  Serial.begin(9600L);

  // Set pin as INPUT pin
  pinMode(pin, INPUT);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  // Read pin value
  int value = analogRead(pin);

  // Filter it
  int filtered = filter.update(value);

  // Print the filtered value
  Serial.println(filtered);
}
