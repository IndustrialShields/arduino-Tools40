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
SimplePacket request;
SimplePacket response;

// Define slave address to communicate with
uint8_t slaveAddress = 1;

////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
	Serial.begin(9600L);

  // Start RS485
  RS485.begin(19200L);
  RS485.setTimeout(20);

  // Start SimpleComm
  SimpleComm.begin(slaveAddress);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  // Get requests
  if (SimpleComm.receive(RS485, request)) {
    int value = request.getInt();

    Serial.print("Received value: ");
    Serial.println(value);

    // Process value
    value++;

    // Send response to the request packet source
    response.setData(value);
    if (SimpleComm.send(RS485, response, request.getSource())) {
      Serial.print("Sent value: ");
      Serial.println(value);
    }
  }
}
