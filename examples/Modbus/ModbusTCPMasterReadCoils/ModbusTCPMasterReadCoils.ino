////////////////////////////////////////////////////////////////////////////////////////////////////
/*
   Copyright (c) 2018 Boot&Work Corp., S.L. All rights reserved

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

#include <ModbusTCPMaster.h>

#if defined(MDUINO_PLUS)
#include <Ethernet2.h>
#else
#include <Ethernet.h>
#endif

// Ethernet configuration values
uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
uint8_t ip[] = { 10, 10, 10, 19 };
uint8_t slaveIp[] = { 10, 10, 10, 1 };
uint16_t slavePort = 502;

// Define the ModbusTCPMaster object
ModbusTCPMaster master;

// Ethernet client object used to connect to the slave
EthernetClient slave;

uint32_t lastSentTime = 0UL;

////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(9600UL);

  // Begin Ethernet
  Ethernet.begin(mac, ip);

  // NOTE: it is not necessary to start the modbus master object
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  // Connect to slave if not connected
  // The ethernet connection is managed by the application, not by the library
  // In this case the connection is opened once
  if (!slave.connected()) {
    slave.stop();

    slave.connect(slaveIp, slavePort);
  }

  // Send a request every 1000ms if connected to slave
  if (slave.connected()) {
    if (millis() - lastSentTime > 1000) {
      // Send a Read Coils request to the slave with address 31
      // It requests for 23 coils starting at address 10
      // IMPORTANT: all read and write functions start a Modbus transmission, but they are not
      // blocking, so you can continue the program while the Modbus functions work. To check for
      // available responses, call master.available() function often.
      if (!master.readCoils(slave, 31, 10, 23)) {
        // Failure treatment
      }

      lastSentTime = millis();
    }

    // Check available responses often
    if (master.isWaitingResponse()) {
      ModbusResponse response = master.available();
      if (response) {
        if (response.hasError()) {
          // Response failure treatment. You can use response.getErrorCode()
          // to get the error code.
        } else {
          // Get the coil value from the response
          bool coil = response.isCoilSet(0);

          // Treat the rest of coils values...
          Serial.print("Coil value: ");
          Serial.println(coil);
        }
      }
    }
  }
}
