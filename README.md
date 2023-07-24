# Tools40 library
### by [Industrial Shields](https://www.industrialshields.com)

Tools40 implements some common functions and modules on industrial environment firmwares for Arduino and ESP32 based devices.

***Both Pulses, ModbusTCPMaster and ModbusTCPSlave are only available for Industrial Shields devices, but you can modify them to be used in other devices***.

## Gettings started

### Prerequisites
1. The [Arduino IDE](http://www.arduino.cc) 1.8.0 or higher
2. The [Industrial Shields Arduino boards](https://www.industrialshields.com/blog/industrial-shields-blog-1/post/installing-the-industrial-shields-boards-in-the-arduino-ide-63) or the [Industrial Shields ESP32 boards](https://www.industrialshields.com/blog/industrial-shields-blog-1/post/installing-the-industrial-shields-boards-in-the-arduino-ide-63) equivalent (optional, used in the examples).

### Installing
1. Download the [library](http://www.github.com/industrialshields/arduino-tools40) from the GitHub as a "ZIP" file.
2. From the Arduino IDE, select the downloaded "ZIP" file in the menu "Sketch/Include library/Add .ZIP library".
3. Now you can open any example from the "File/Examples/Tools40" menu.

## Reference
Tools40 contains different modules:

1. [SimpleComm](#simplecomm)
2. [Filter](#filter)
3. [Timer](#timer)
4. [Pulses](#pulses)
5. [Counter](#counter)
6. [ModbusRTUMaster](#modbusrtumaster)
7. [ModbusTCPMaster](#modbustcpmaster)
8. [ModbusTCPSlave](#modbustcpslave)

### SimpleComm

```c++
#include <SimpleComm.h>
```

The **SimpleComm** module sends and receives data through any Stream: RS-485, RS-232, Ethernet... It is enough flexible to support different kind of communication typologies: Ad-hoc, Master-Slave, Client-Server, and so on, using an easy to use API. 

The **SimplePacket** class encapsulates the data into a packet.

The `setData` function fills up the packet with the desired data for sending it to a remote device. It is possible to fill up the packet with different types of data: bool, char, unsigned char, int, unsigned int, long, unsigned long, double, string and even custom data types.

```c++
int num = 1234;
SimplePacket packet1;
packet1.setData(num);
```

```c++
SimplePacket packet2;
packet2.setData("text data");
```

```c++
typedef struct {
    int a;
    char b;
} customType;
customType customVar;
customVar.a = 1234;
customVar.b = 'B';
SimplePacket packet3;
packet3.setData(customVar, sizeof(customVar));
```

There are also getter functions, which return the data contained in the packet, depending on the data type. If length is specified, it returns the data length in bytes.

```c++
int num = packet1.getInt();
```

```c++
const char *str = packet2.getString();
```

```c++
const customType *var = (const customType*) packet3.getData();
```

The **SimpleComm** class is the interface for sending and receiving packets through the desired Stream.

The `begin(byte)` function enables the communication system and sets the devices identifier/address. Each device has its own address which identifies it. Devices receive packets sent to them, using their address, but not to others.

```c++
byte address = 1234;
SimpleComm.begin(address);
```

It is possible to send packets to a device with an address, and optionally define the packet type.

```c++
byte destination = 87;
SimpleComm.send(RS485, packet, destination);
SimpleComm.send(RS485, packet, destination, 0x33);
```

To send packets to all devices it is possible to send a broadcast packet with the destination field set to 0 (zero).

```c++
byte destination = 0;
SimpleComm.send(RS485, packet, destination);
```

The `receive` function receives a packet from another device, using the stream. It returns true if a packet is really received.

```c++
SimplePacket rxPacket;
if (SimpleComm.receive(RS485, rxPacket)) {
    // A packet is received
}
```

It is also possible to receive all the packets (sniffer mode) by setting the SimpleComm address to 0 (zero). This mode is useful when implementing a gateway node between Ethernet and RS-485.

```c++
byte address = 0;
SimpleComm.begin(address);
```

### Filter

```c++
#include <Filter.h>
```

The filter module includes filtering software used to smooth analog inputs. This is really useful when you have an analog signal that is unstable.

The definition of the analog filter requires a number of samples and a sample period.
```c++
AnalogFilter<10, 2> aFilter;
```
In this example, the sample time is 2ms and the number of samples is 10.

It is also possible to call the constructor of the filter with an initial value.
```c++
AnalogFilter<10, 2> aFilter(24000);
```
Here the initial value of the filter is 24000, instead of 0 (the default).

The `update` function returns the filtered value according to the passed input value as argument.

```c++
int input = analogRead(I0_0);
int filteredInput = aFilter.update(input);
```

### Timer

```c++
#include <Timer.h>
```

The Timer module provides three different kind of Timers, based on standard timers schemes:

```c++
PulseTimer PT(1000);
OnDelayTimer TON(1000);
OffDelayTimer TOF(1000);
```

The `PulseTimer` returns HIGH during the defined time when a rising edge in the input is generated.

```c++
int in = digitalRead(I0_0);
if (PT.update(in) == HIGH) {
    // Enter here during 1000ms after in is rised
}
```

The `OnDelayTimer` returns HIGH when the input is HIGH during the defined time and until the input falls.

```c++
int in = digitalRead(I0_0);
if (TON.update(in) == HIGH) {
    // Enter here after I0.0 is HIGH during 1000ms and until it becomes LOW
}
```

The `OffDelayTimer` returns LOW when the input is LOW during the defined time and until the input rises.

```c++
int in = digitalRead(I0_0);
if (TOFF.update(in) == HIGH) {
    // Enter here after I0.0 is LOW during 1000ms and until it becomes HIGH
}
```

### Pulses

**WARNING: This library is not available for ESP32, as it uses the Arduino timers.**

```c++
#include <Pulses.h>
```

The Pulses module provides functions for starting and stopping a train of pulses at the desired frequency.

The `startPulses(pin, frequency, precision)` function starts the train of pulses at the specified frequency and precision. The default frequency is 1kHz and the default precision is 3.

```c++
pinMode(3, OUTPUT);
startPulses(3, 2000, 3);
```

The `stopPulses(Pin)` function stops the train of pulses.

```c++
stopPulses(3);
```

On ARDBOX Analog it is possible to use this functions in outputs:

* TIMER0: Q0.1 and Q0.6
* TIMER1: Q0.2 and Q0.3
* TIMER3: Q0.5

On MDUINO-21, MDUINO-42 and MDUINO-58 it is possible to use this functions in outputs:

* TIMER0: Q0.5 and Q2.6
* TIMER1: Q2.5
* TIMER2: Q1.5 (Multiply the frequency x2)
* TIMER3: PIN2, PIN3 and Q0.6
* TIMER4: Q0.7, Q1.6 and Q1.7
* TIMER5: Q1.3, Q1.4 and Q2.0

**IMPORTANT**: It is not possible to have different frequencies between the same TIMER Pin’s. Some outputs share the same timer, so they work at the same frequency.

**CAUTION!!!** When the TIMER0 pins are used, all the time functions change their functionality as `delay()`, `millis()`,`micros()`,`delayMicroseconds()` and others.

Next it is showed recommended precision between different frequencies:

* Precision = 1: from 30Hz to 150Hz
* Precision = 2: from 150Hz to 500Hz
* Precision = 3: from 500Hz to 4kHz
* Precision = 4: from 4kHz to 32kHz
* Precision = 5: from 32kHz to 4MHz

To have a high precision on the desired frequency, it is recommended to use the closer precision to the values of the previous table.

### Counter

```c++
#include <Counter.h>
```

The Counter module provides a way of counting rising edges until a defined preset value.

```c++
Counter C(100);
```

The `Counter` returns HIGH when the internal counter is equal to the preset value.

```c++
int up = digitalRead(I0_0);
int down = digitalRead(I0_1);
int reset = digitalRead(I0_2);
if (C.update(up, down, reset, preset) == HIGH) {
    // Enter here when the counter is equal to 100
	// The counter counts up when I0.0 rises
	// The counter counts down when I0.1 rises
	// The counter is set to zero when I0.2 is HIGH
}
```

### ModbusRTUMaster

The ModbusRTUMaster module implements the Modbus RTU Master capabilities.

```c++
#include <ModbusRTUMaster.h>
```

It is possible to use any hardware Serial stream:

* RS-485

```c++
#include <RS485.h>

ModbusRTUMaster master(RS485);
```

* RS-232

```c++
#include <RS232.h>

ModbusRTUMaster master(RS232);
```

Before using it, it is required to call the begin function in the setup for both the serial port and the Modbus variable. It is a good practise to set the baudrate (default: 19200bps) also in the Modbus variable to define the Modbus internal timeouts.

```c++
RS485.begin(9600, HALFDUPLEX, SERIAL_8E1);
master.begin(9600);
```

The functions to read and write slave values are:

```c++
readCoils(slave_address, address, quantity);
readDiscreteInputs(slave_address, address, quantity);
readHoldingRegisters(slave_address, address, quantity);
readInputRegisters(slave_address, address, quantity);
writeSingleCoil(slave_address, address, value);
writeSingleRegister(slave_address, address, value);
writeMultipleCoils(slave_address, address, values, quantity);
writeMultipleRegisters(slave_address, address, values, quantity);
```

Where
* `slave_address` is the Modbus RTU slave address.
* `address` is the coil, digital input, holding register or input register address. Usually this address is the coil, digital input, holding register or input register number minus 1: the holding register number `40009` has the address `8`.
* `quantity` is the number of coils, digital inputs, holding registers or input registers to read/write.
* `value` is the given value of the coil or holding registers on a write operation. Depending on the function the data type changes. A coil is represented by a `bool` value and a holding register is represented by a `uint16_t` value.

On a multiple read/write function the `address` argument is the first element address.
On a multiple write function the `values` argument is an array of values to write.

It is important to notice that these functions are non-blocking, so they don't return the read value. They return `true` or `false` depending on the current module state. If there is a pending Modbus request, they return `false`.

```c++
// Read 5 holding registers from address 0x24 of slave with address 0x10
if (master.readHoldingRegisters(0x10, 0x24, 5)) {
	// OK, the request is being processed
} else {
	// ERROR, the master is not in an IDLE state
}
```

There is the `available()` function to check for responses from the slave.

```c++
ModbusResponse response = master.available();
if (response) {
	// Process response
}
```

The `ModbusResponse` implements some functions to get the response information:

```c++
hasError();
getErrorCode();
getSlave();
getFC();
isCoilSet(offset);
isDiscreteInputSet(offset);
isDiscreteSet(offset);
getRegister(offset);
```

```c++
ModbusResponse response = master.available();
if (response) {
	if (response.hasError()) {
		// There is an error. You can get the error code with response.getErrorCode()
	} else {
		// Response ready: print the read holding registers
		for (int i = 0; i < 5; ++i) {
			Serial.println(response.getRegister(i);
		}
	}
}
```

The possible error codes are:

```
0x01 ILLEGAL FUNCTION
0x02 ILLEGAL DATA ADDRESS
0x03 ILLEGAL DATA VALUE
0x04 SERVER DEVICE FAILURE
```

### ModbusTCPMaster

***Both ModbusTCPMaster and ModbusTCPSlave are only available for Industrial Shields devices, but you can modify them to be used in other Arduino or ESP32 devices***

The ModbusTCPMaster module implements the Modbus TCP Master capabilities.

```c++
#include <ModbusTCPMaster.h>

ModbusTCPMaster master;
```

The ModbusTCPMaster module uses the EthernetClient to send requests to the slaves:

```c++
#include <Ethernet.h>

EthernetClient client;
```

The functions to read and write slave values are:

```c++
readCoils(client, slave_address, address, quantity);
readDiscreteInputs(client, slave_address, address, quantity);
readHoldingRegisters(client, slave_address, address, quantity);
readInputRegisters(client, slave_address, address, quantity);
writeSingleCoil(client, slave_address, address, value);
writeSingleRegister(client, slave_address, address, value);
writeMultipleCoils(client, slave_address, address, values, quantity);
writeMultipleRegisters(client, slave_address, address, values, quantity);
```

Where
* `client` is the EthernetClient connected to the slave.
* `slave_address` is the Modbus RTU slave address.
* `address` is the coil, digital input, holding register or input register address. Usually this address is the coil, digital input, holding register or input register number minus 1: the holding register number `40009` has the address `8`.
* `quantity` is the number of coils, digital inputs, holding registers or input registers to read/write.
* `value` is the given value of the coil or holding registers on a write operation. Depending on the function the data type changes. A coil is represented by a `bool` value and a holding register is represented by a `uint16_t` value.

On a multiple read/write function the `address` argument is the first element address.
On a multiple write function the `values` argument is an array of values to write.

It is important to notice that these functions are non-blocking, so they don't return the read value. They return `true` or `false` depending on the current module state. If there is a pending Modbus request or the client is not connected, they return `false`.

```c++
// Read 5 holding registers from address 0x24 of slave with address 0x10
if (master.readHoldingRegisters(client, 0x10, 0x24, 5)) {
	// OK, the request is being processed
} else {
	// ERROR, the master is not in an IDLE state
}
```

There is the `available()` function to check for responses from the slave.

```c++
ModbusResponse response = master.available();
if (response) {
	// Process response
}
```

The `ModbusResponse` implements some functions to get the response information:

```c++
hasError();
getErrorCode();
getSlave();
getFC();
isCoilSet(offset);
isDiscreteInputSet(offset);
isDiscreteSet(offset);
getRegister(offset);
```

```c++
ModbusResponse response = master.available();
if (response) {
	if (response.hasError()) {
		// There is an error. You can get the error code with response.getErrorCode()
	} else {
		// Response ready: print the read holding registers
		for (int i = 0; i < 5; ++i) {
			Serial.println(response.getRegister(i);
		}
	}
}
```

The possible error codes are:

```
0x01 ILLEGAL FUNCTION
0x02 ILLEGAL DATA ADDRESS
0x03 ILLEGAL DATA VALUE
0x04 SERVER DEVICE FAILURE
```

### ModbusTCPSlave

***Both ModbusTCPMaster and ModbusTCPSlave are only available for Industrial Shields devices, but you can modify them to be used in other Arduino or ESP32 devices***

The ModbusTCPSlave module implements the Modbus TCP Slave capabilities.

```c++
#include <ModbusTCPSlave.h>

ModbusTCPSlave slave;
```

The default TCP port is the `502` but you can change it with:

```c++
// Set the TCP listening port to 510 instead of 502
ModbusTCPSlave slave(510);
```

To map the coils, discrete inputs, holding registers and input registers addresses with the desired variables values, the module uses four variables arrays:

```c++
bool coils[NUM_COILS];
bool discreteInputs[NUM_DISCRETE_INPUTS];
uint16_t holdingRegistesr[NUM_HOLDING_REGISTERS];
uint16_t inputRegisters[NUM_INPUT_REGISTERS];
```

The lengths of these arrays depend on the application and the registers usages. Obviously, the names of the arrays also depend on your preferences.

To associate the registers arrays with the library it is possible to use these functions in the `setup`:

```c++
slave.setCoils(coils, NUM_COILS);
slave.setDiscreteInputs(discreteInputs, NUM_DISCRETE_INPUTS);
slave.setHoldingRegisters(holdingRegisters, NUM_HOLDING_REGISTERS);
slave.setInputRegisters(inputRegisters, NUM_INPUT_REGISTERS);
```

It is not required to have all kind of registers mapping to work, only the used by the application.

To start the ModbusTCP server, call the `begin` function after the registers mapping. It is also possible to call the `begin` function before the registers mapping. Remember to begin the Ethernet before the ModbusTCPSlave object in the `setup`.

```c++
// Init the Ethernet
Ethernet.begin(mac, ip);

// Init the ModbusTCPSlave object
slave.begin();
```

At this time the ModbusTCP server is runnning and the only important thing to do is to update the ModbusTCPSlave object often in the `loop` function, and tret the registers mapping values to update variables, inputs and outputs.

```c++
// Update discrete inputs and input registers values
discreteInputs[0] = digitalRead(I0_7);
inputRegisters[0] = analogRead(I0_0);
// ...

// Update the ModbusTCPSlave object
slave.update();

// Update coils and holding registers
digitalWrite(Q0_0, coils[0]);
// ...
```
