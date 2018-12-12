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

#if defined(MDUINO_PLUS)
#include <Ethernet2.h>
#else
#include <Ethernet.h>
#endif

#include <SimpleComm.h>

// Create SimplePacket for sending and receiving data
SimplePacket packet;

// Define Ethernet values
byte mac[] = {0xDE, 0xAD, 0xBE, 0x75, 0x24, 0x02};

// TCP client
EthernetClient client;

// Define server info
unsigned short serverTCPPort = 64321;
byte serverIPAddress[] = {192, 168, 1, 5};

// Define SimpleComm addresses
uint8_t serverAddress = 0;
uint8_t myAddress = 1;

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

  // Connect to server and send data
  if (client.connect(serverIPAddress, serverTCPPort)) {
    Serial.println("Connected to server");

    // Prepare data to send
    packet.setData("text message");
    
    // Send data to the server
    if (SimpleComm.send(client, packet, serverAddress)) {
      Serial.println("Data sent to server");
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  // Wait response
  if (client.available()) {
    if (SimpleComm.receive(client, packet)) {
      Serial.println("Data received from server:");
      Serial.println(packet.getString());
    }
  }

  // On server disconnection
  if (!client.connected()) {
    Serial.println("Disconnected from server");

    // Clear client
    client.stop();

    // And do nothing
    while (true);
  }
}
