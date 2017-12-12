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
// Filtering time: 200ms
DigitalFilter<200> filter;
const int pin = I0_0;

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
  int value = digitalRead(pin);

  // Filter it
  int filtered = filter.update(value);

  // Print the filtered value
  Serial.println(filtered);
}
