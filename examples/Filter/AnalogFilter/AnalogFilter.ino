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

////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  // Init Serial port
  Serial.begin(9600L);

  // Set I0.7 as INPUT pin
  pinMode(I0_7, INPUT);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  // Read I0.7 value
  int i0_7 = analogRead(I0_7);

  // Filter it
  int filtered = filter.update(i0_7);

  // Print the filtered value
  Serial.println(filtered);
}
