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

#include <Ethernet.h>
#include <SimpleComm.h>

// Create SimplePacket for receiving and replying data
SimplePacket packet;

// Define Ethernet values
byte mac[] = {0xDE, 0xAD, 0xBE, 0x75, 0x24, 0x01};

// TCP server for receiving messages
EthernetServer server(64321);

// Define my SimpleComm address
uint8_t myAddress = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
	Serial.begin(9600L);

  // Start SimpleComm
  SimpleComm.begin(myAddress);
  Serial.print("SimpleComm address: ");
  Serial.println(myAddress, HEX);

  // Start Ethernet interface using DCHP for getting an IP address
  Ethernet.begin(mac);
  Serial.print("IP address: ");
  Serial.println(Ethernet.localIP());

  // Start TCP server
  server.begin();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  // Wait for TCP connections
  EthernetClient client = server.available();
  if (client) {
    // Receive packets from the connected client
    if (SimpleComm.receive(client, packet)) {
      uint8_t source = packet.getSource();

      Serial.println("Received packet:");
      Serial.print("\tFrom: ");
      Serial.println(source, HEX);
      Serial.print("\tType: ");
      Serial.println(packet.getType(), HEX);

      if (SimpleComm.send(client, packet, source)) {
        Serial.println("Echo sent");
      }

      // Force closing client connection
      client.stop();
    }
  }
}
