// Counter library example
// by Industrial Shields

#include <Counter.h>

// Define a new Counter with preset value to 5
Counter counter(5);
int inputUpPin = I0_0;
int inputDownPin = I0_1;
int inputResetPin = I0_2;

#if defined(ARDBOX_ANALOG) || defined(ARDBOX_ANALOG_HF)
int outputPin = Q0_0;
#elif defined(ARDBOX_RELAY) || defined(ARDBOX_RELAY_HF)
int outputPin = R3;
#elif defined(MDUINO_19R) || defined(MDUINO_38R) || defined(MDUINO_57R) || \
  defined(MDUINO_19R_PLUS) || defined(MDUINO_38R_PLUS) || defined(MDUINO_57R_PLUS)
int outputPin = R0_1;
#elif defined(MDUINO_21) || defined(MDUINO_42) || defined(MDUINO_58) || \
  defined(MDUINO_21_PLUS) || defined(MDUINO_42_PLUS) || defined(MDUINO_58_PLUS)
int outputPin = Q0_0;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  // Configure inputUpPin and inputDown as inputs
  pinMode(inputUpPin, INPUT);
  pinMode(inputDownPin, INPUT);
  pinMode(inputResetPin, INPUT);

  // Configure outputPin as output
  pinMode(outputPin, OUTPUT);
  digitalWrite(outputPin, LOW);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  // Read inputUpPin
  int inputUp = digitalRead(inputUpPin);
  int inputDown = digitalRead(inputDownPin);
  int inputReset = digitalRead(inputResetPin);

  // Update counter using inputUpPin, inputDownPin and inputResetPin values to count
  // up and down. Then set outputPin associated variable value.
  // The output variable value is HIGH when the counter value is equal to the preset.
  int output = counter.update(inputUp, inputDown, inputReset);

  // Update outputPin with its associated variable value
  digitalWrite(outputPin, output);
}
