#include <Arduino.h>

void setup()
{
  // Initialize GPIO21 as an output
  pinMode(21, OUTPUT);
  // Start the serial communication
  Serial.begin(9600);
}

void loop()
{
  // Check if data is available on the serial port
  if (Serial.available() > 0)
  {
    // Read the command from the serial input
    String command = Serial.readStringUntil('\n');

    // Toggle LED based on the command
    if (command == "TOGGLE_ON")
    {
      digitalWrite(21, HIGH);   // Turn the LED on
      Serial.println("LED_ON"); // Acknowledge
    }
    else if (command == "TOGGLE_OFF")
    {
      digitalWrite(21, LOW);     // Turn the LED off
      Serial.println("LED_OFF"); // Acknowledge
    }
  }
}
