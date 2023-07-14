/*
   Copyright (c) 2023 Boot&Work Corp., S.L. All rights reserved

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

#include "PacketTypes.h"

// Create SimplePacket for receiving data
SimplePacket packet;

// Define the slave address
const uint8_t slaveAddress = 1;

////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  // Start Serial Port
  Serial.begin(9600);

  // Start RS-485
  RS485.begin(19200);

  // Start SimpleComm
  SimpleComm.begin();
}

// Template function to send data
template <typename T> bool sendDataPacket(packet_t t, T value, const char* msg) {
    packet.setData(value);
    packet.setType(t);
    if (!SimpleComm.send(RS485, packet, slaveAddress)) {
      Serial.println(msg);
      return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  static unsigned long lastSent = millis();

  // Send packet periodically: once per second
  if (millis() - lastSent >= 5000) {

    sendDataPacket(P_DOUBLE, (SP_DOUBLE) 3.1415, "Error sending Double");

    sendDataPacket(P_ULONG, (SP_ULONG) 45677213, "Error sending Unsigned Long");

    sendDataPacket(P_LONG, (SP_LONG) -2098765413, "Error sending Long");

    sendDataPacket(P_UINT, (SP_UINT) 453, "Error sending Unsigned Int");

    sendDataPacket(P_INT, (SP_INT) -99, "Error sending Int");

    sendDataPacket(P_UCHAR, (SP_UCHAR) 56, "Error sending Unsigned Char");

    sendDataPacket(P_CHAR, (SP_CHAR) 199, "Error sending Char");

    if (sendDataPacket(P_BOOL, (SP_BOOL) true, "Error sending Bool")) {
      lastSent = millis();
    }
  }
}
