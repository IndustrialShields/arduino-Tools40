# Tools40 library

Tools40 implements some common functions and modules on industrial environment firmwares for Arduino based devices.

## Gettings started

### Prerequisites
1. The [Arduino IDE](http://www.arduino.cc) 1.8.0 or higher

### Installing
2. Download the [library](http://www.github.com/industrialshields/arduino-tools40) from the GitHub as a "ZIP" file.
2. From the Arduino IDE, select the downloaded "ZIP" file in the menu "Sketch/Inlcude library/Add .ZIP library".
2. Now you can open any example from the "File/Examples/Tools40" menu.

## Reference
Tools40 contains different modules:

### SimpleComm

```c++
#include <SimpleComm.h>
```

The **SimpleComm** module sends and receives data through any Arduino Stream: RS-485, RS-232, Ethernet... It is enough flexible to support different kind of communication typologies: Ad-hoc, Master-Slave, Client-Server, and so on, using an easy to use API. 

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

The **SimpleComm** class is the interface for sending and receiving packets through the desired Arduino Stream.

The `begin(byte)` function enables the communication system and sets the devices identifier/address. Each device has its own address which identifies it. Devices receive packets sent to them, using their address, but not to others.

```c++
byte address = 1234;
SimpleComm.begin(address);
```

It is possible to send packets to a destination device, and optionally define the packet type.

```c++
byte destination = 0;
SimpleComm.send(RS485, packet, destination);
SimpleCOmm.send(RS485, packet, destination, 0x33);
```

The `receive` function receives a packet from another device, using the stream. It returns true if a packet is really received.

```c++
SimplePacket rxPacket;
if (SimpleCommreceive(RS485, rxPacket)) {
    // A packet is received
}
```

### Filter

```c++
#include <AnalogFilter.h>
```

The filter module includes filtering software used to smooth analog inputs. This is really useful when you have an analog signal that is unstable.

The definition of the analog filter requires a number of samples and a sample period.
```c++
AnalogFilter<10, 2> aFilter;
```
In this example, the sample time is 2ms and the number of samples is 10.

The `update` function returns the filtered value according to the passed input value as argument.

```c++
int input = analogRead(I0_0);
aFilter.update(input);
```

### Timer

```c++
#include <PulseTimer.h>
#include <OnDelayTimer.h>
#include <OffDelayTimer.h>
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


