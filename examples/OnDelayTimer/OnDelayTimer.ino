// Timer library example
// by Industrial Shields

#include <Timer.h>

// Define a new OnDelayTimer with preset time of 3s
OnDelayTimer timer(3000);

////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  // Configure I0.7 as input
  pinMode(I0_7, INPUT);

  // Configure Q0.0 as output
  pinMode(Q0_0, OUTPUT);
  digitalWrite(Q0_0, LOW);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  // Read I0.7
  int i0_7 = digitalRead(I0_7);

  // Update timer using I0.7 value and set Q0.0 associated variable value
  int q0_0 = timer.update(i0_7);

  // Update Q0.0 with its associated variable value
  digitalWrite(Q0_0, q0_0);
}
