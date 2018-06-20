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

#include <ModbusRTUMaster.h>
#include <RS485.h>

// Define the ModbusRTUMaster object, using the RS-485 port
ModbusRTUMaster master(RS485);

uint32_t lastSentTime = 0UL;

////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  // Start the modbus master object.
  // It is possible to define the port rate (default: 19200)
  // and the serial frame mode (default: 8N1)
  master.begin();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  // Send a request every 1000ms
  if (millis() - lastSentTime > 1000) {
    // Send a Read Coils request to the slave with address 31
    // It requests for 23 coils starting at address 10
    // IMPORTANT: all read and write functions start a Modbus transmission, but they are not
    // blocking, so you can continue the program while the Modbus functions work. To check for
    // available responses, call master.available() function often.
    if (!master.readCoils(31, 10, 23)) {
      // Failure treatment
    }

    lastSentTime = millis();
  }

  // Check available responses often
  if (master.isWaitingResponse()) {
    ModbusRTCResponse response = master.available();
    if (response) {
      if (response.hasError()) {
        // Response failure treatment. You can use response.getErrorCode()
        // to get the error code.
      } else {
        // Get the coil value from the response
        bool coil = reponse.isCoilSet(0);

        // Treat the rest of coils values...
      }
    }
  }
}
