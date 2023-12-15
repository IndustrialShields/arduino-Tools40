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

#include <ModbusRTUSlave.h>

// Modbus registers mapping
// This example uses the M-Duino 21+ mapping
int digitalOutputsPins[] = {
#if defined(PIN_Q0_4)
  Q0_0, Q0_1, Q0_2, Q0_3, Q0_4,
#endif
};
int digitalInputsPins[] = {
#if defined(PIN_I0_6)
  I0_0, I0_1, I0_2, I0_3, I0_4, I0_5, I0_6,
#endif
};
int analogOutputsPins[] = {
#if defined(PIN_Q0_7)
  A0_5, A0_6, A0_7,
#endif
};
int analogInputsPins[] = {
#if defined(PIN_I0_12)
  I0_7, I0_8, I0_9, I0_10, I0_11, I0_12,
#endif
};

#define numDigitalOutputs int(sizeof(digitalOutputsPins) / sizeof(int))
#define numDigitalInputs int(sizeof(digitalInputsPins) / sizeof(int))
#define numAnalogOutputs int(sizeof(analogOutputsPins) / sizeof(int))
#define numAnalogInputs int(sizeof(analogInputsPins) / sizeof(int))

bool digitalOutputs[numDigitalOutputs];
bool digitalInputs[numDigitalInputs];
uint16_t analogOutputs[numAnalogOutputs];
uint16_t analogInputs[numAnalogInputs];

// Define the ModbusRTUSlave object with Modbus RTU slave address: 31,
// using the RS-485, RS-232 or Serial1 port, depending on availability
#if defined HAVE_RS485_HARD
#include <RS485.h>
ModbusRTUSlave modbus(RS485, 31);

#elif defined HAVE_RS232_HARD
#include <RS232.h>
ModbusRTUSlave modbus(RS232, 31);

#else
ModbusRTUSlave modbus(Serial1, 31);
#endif

const uint32_t baudrate = 38400UL;

////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(9600UL);

  // Init variables, inputs and outputs
  for (int i = 0; i < numDigitalOutputs; ++i) {
    digitalOutputs[i] = false;
    digitalWrite(digitalOutputsPins[i], digitalOutputs[i]);
  }
  for (int i = 0; i < numDigitalInputs; ++i) {
    digitalInputs[i] = digitalRead(digitalInputsPins[i]);
  }
  for (int i = 0; i < numAnalogOutputs; ++i) {
    analogOutputs[i] = 0;
    analogWrite(analogOutputsPins[i], analogOutputs[i]);
  }
  for (int i = 0; i < numAnalogInputs; ++i) {
    analogInputs[i] = analogRead(analogInputsPins[i]);
  }

  // Start the serial port
#if defined HAVE_RS485_HARD
  RS485.begin(baudrate, HALFDUPLEX, SERIAL_8E1);
#elif defined HAVE_RS232_HARD
  RS232.begin(baudrate, SERIAL_8E1);
#else
  Serial1.begin(baudrate, SERIAL_8E1);
#endif

  // Init ModbusRTUSlave object
  modbus.begin(baudrate);

  modbus.setCoils(digitalOutputs, numDigitalOutputs);
  modbus.setDiscreteInputs(digitalInputs, numDigitalInputs);
  modbus.setHoldingRegisters(analogOutputs, numAnalogOutputs);
  modbus.setInputRegisters(analogInputs, numAnalogInputs);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  // Update inputs
  for (int i = 0; i < numDigitalInputs; ++i) {
    digitalInputs[i] = digitalRead(digitalInputsPins[i]);
  }
  for (int i = 0; i < numAnalogInputs; ++i) {
    analogInputs[i] = analogRead(analogInputsPins[i]);
  }

  // Process modbus requests
  modbus.update();

  // Update outputs
  for (int i = 0; i < numDigitalOutputs; ++i) {
    digitalWrite(digitalOutputsPins[i], digitalOutputs[i]);
  }
  for (int i = 0; i < numAnalogOutputs; ++i) {
    analogWrite(analogOutputsPins[i], analogOutputs[i]);
  }
}
