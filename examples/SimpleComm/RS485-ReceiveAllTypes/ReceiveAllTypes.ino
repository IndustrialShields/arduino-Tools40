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

// Define my SimpleComm address
const uint8_t myAddress = 1;

////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  // Start Serial Port
  Serial.begin(9600);

  // Start RS-485
  RS485.begin(19200);

  // Start SimpleComm
  SimpleComm.begin(myAddress);
}

// Template function to receive
template <typename F> bool receiveDataPacket(packet_t t, F f) {
  while (!SimpleComm.receive(RS485, packet));
  packet_t packet_type = (packet_t) packet.getType();
  const char* type_string = packet_type_to_string(t);
  if (packet_type != t) {
    Serial.print("Expected "); Serial.print(type_string);
    Serial.print(" got "); Serial.println(packet_type_to_string(packet_type));
  }
  else {
    Serial.print("Got "); Serial.print(type_string);
    Serial.print(": "); Serial.println(f());
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  receiveDataPacket(P_DOUBLE, [&]{ return packet.getDouble(); });

  receiveDataPacket(P_ULONG, [&]{ return packet.getULong(); });

  receiveDataPacket(P_LONG, [&]{ return packet.getLong(); });

  receiveDataPacket(P_UINT, [&]{ return packet.getUInt(); });

  receiveDataPacket(P_INT, [&]{ return packet.getInt(); });

  receiveDataPacket(P_UCHAR, [&]{ return packet.getUChar(); });

  receiveDataPacket(P_CHAR, [&]{ return (uint8_t) packet.getChar(); });

  receiveDataPacket(P_BOOL, [&]{ return packet.getBool(); });
}