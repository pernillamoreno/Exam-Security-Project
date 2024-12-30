/*just for test */
#include <Arduino.h>

void setup()
{
  // Initialize GPIO21 as an output
  pinMode(21, OUTPUT);
}

void loop()
{
  // Turn the LED on
  digitalWrite(21, HIGH);
  delay(1000); // Wait for 1 second

  // Turn the LED off
  digitalWrite(21, LOW);
  delay(1000); // Wait for 1 second
}
