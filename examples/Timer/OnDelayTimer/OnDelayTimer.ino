// Timer library example
// by Industrial Shields

#include <Timer.h>

// Define a new OnDelayTimer with preset time of 3s
OnDelayTimer timer(3000);
int inputPin = I0_0;

#if defined(ARDBOX_ANALOG)
int outputPin = Q0_0;
#elif defined(ARDBOX_RELAY)
int outputPin = R3;
#elif defined(MDUINO_19R) || defined(MDUINO_38R) || defined(MDUINO_57R)
int outputPin = R0_1;
#elif defined(MDUINO_21) || defined (MDUINO_42) || defined(MDUINO_58)
int outputPin = Q0_0;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  // Configure inputPin as input
  pinMode(inputPin, INPUT);

  // Configure outputPin as output
  pinMode(outputPin, OUTPUT);
  digitalWrite(outputPin, LOW);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  // Read inputPin
  int input = digitalRead(inputPin);

  // Update timer using inputPin value and set outputPin associated variable value
  int output = timer.update(input);

  // Update outputPin with its associated variable value
  digitalWrite(outputPin, output);
}
