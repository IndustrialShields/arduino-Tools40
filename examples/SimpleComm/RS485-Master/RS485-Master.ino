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

#include <RS485.h>
#include <SimpleComm.h>

// Create SimplePacket for sending and receiving data
SimplePacket packet;

// Define master address
uint8_t masterAddress = 0;

// Define slave address to communicate with
uint8_t slaveAddress = 1;

// Value to send as packet data
int value = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
	Serial.begin(9600L);

  // Start RS485
  RS485.begin(19200L);
  RS485.setTimeout(20);

  // Start SimpleComm
  SimpleComm.begin(masterAddress);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  static unsigned long lastSent = millis();

  // Send packet periodically: once per second
  if (millis() - lastSent >= 1000) {
    // Set request packet data
    packet.setData(value);

    // Send request to slave
    if (SimpleComm.send(RS485, packet, slaveAddress)) {
      lastSent = millis();

      Serial.print("Sent value: ");
      Serial.println(value);
    }
  }

  // Get responses
  if (SimpleComm.receive(RS485, packet)) {
    // Update value from the response
    value = packet.getInt();

    Serial.print("Received value: ");
    Serial.println(value);
  }
}
