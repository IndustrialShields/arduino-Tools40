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

// You can define different types of messages.
// Here is an example:
#define PRINT_TEXT_TYPE     0x01 // Used for receiving text and print it
#define DIGITAL_OUTPUT_TYPE 0x02 // Used for setting the digital output Q0.0
#define DIGITAL_INPUT_TYPE  0x03 // Used for printing the value of the digital input I0.0

// Create SimplePacket for receiving data
SimplePacket packet;

// Define Ethernet values
byte mac[] = {0xDE, 0xAD, 0xBE, 0x75, 0x24, 0x01};

// TCP server for receiving messages
EthernetServer server(64321);

// Define my SimpleComm address
uint8_t myAddress = 1;

////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
	Serial.begin(9600L);

  // Set IOs configuration
  pinMode(Q0_0, OUTPUT);
  pinMode(I0_0, INPUT);

  // Start SimpleComm
  SimpleComm.begin(myAddress);

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
      Serial.print("Received packet from: ");
      Serial.println(packet.getSource(), HEX);

      // Depending on the received packet type, do one of the actions below
      switch (packet.getType()) {
        case PRINT_TEXT_TYPE:
          // Print the received data as a text value
          Serial.print("Text: ");
          Serial.println(packet.getString());
          break;

        case DIGITAL_OUTPUT_TYPE:
          // Set Q0.0 value depending on the received data (as a boolean value)
          // true: HIGH, false: LOW
          Serial.print("Set Q0.0 to ");
          Serial.println(packet.getBool() ? "HIGH" : "LOW");
          digitalWrite(Q0_0, packet.getBool() ? HIGH : LOW);
          break;

        case DIGITAL_INPUT_TYPE:
          // Print I0.0 value: HIGH or LOW
          Serial.print("I0.0 value: ");
          Serial.println(digitalRead(I0_0) == HIGH ? "HIGH" : "LOW");
          break;

        default:
          // Treat unknown packet type too
          Serial.print("Received unknown packet type: ");
          Serial.println(packet.getType());
          break;
      }
    }
  }
}
