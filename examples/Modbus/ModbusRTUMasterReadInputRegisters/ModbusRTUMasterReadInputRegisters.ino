/*
   Copyright (c) 2018 Boot&Work Corp., S.L. All rights reserved
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as 
   published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.
   You should have received a copy of the GNU Lesser General Public License
   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <ModbusRTUMaster.h> // Define the ModbusRTUMaster object, using the RS-485 or RS-232 port (depending on availability)

#if defined HAVE_RS485_HARD
#include <RS485.h>
ModbusRTUMaster master(RS485);
#elif defined HAVE_RS232_HARD
#include <RS232.h>
ModbusRTUMaster master(RS232);
#else
ModbusRTUMaster master(Serial1);
#endif

#define REGISTERS_TO_READ 1
uint32_t lastSentTime = 0UL;
const uint32_t baudrate = 38400UL;

//////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(9600UL); // Start the serial port
#if defined HAVE_RS485_HARD
  RS485.begin(baudrate, HALFDUPLEX, SERIAL_8E1);
#elif defined HAVE_RS232_HARD
  RS232.begin(baudrate, SERIAL_8E1);
#else
  Serial1.begin(baudrate, SERIAL_8E1);
#endif

  // Start the modbus master object.
  // It is possible to define the port rate (default: 19200)
  master.begin(baudrate);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  // Send a request every 1000ms
  if (millis() - lastSentTime > 1000) {
    // Send a Read Input Register request to the slave with address 31
    // It requests for REGISTERS_TO_READ registers starting at address 0
    // IMPORTANT: all read and write functions start a Modbus transmission, but they are not
    // blocking, so you can continue the program while the Modbus functions work. To check for
    // available responses, call master.available() function often.
    if (!master.readInputRegisters(31, 0, REGISTERS_TO_READ)) {
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
        // Get the register value from the response
        Serial.print("Input Register values: ");
        for (int i = 0; i < REGISTERS_TO_READ; ++i) {
          Serial.print(response.getRegister(i));
          Serial.print(',');
        }
        Serial.println();
      }
    }
  }
}
