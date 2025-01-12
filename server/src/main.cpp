#include "session.h"
#include "commnctn.h"
#include <Arduino.h>

#define TOGGLE_RELAY 0x03
#define LED_ON 0x01
#define LED_OFF 0x00

void setup()
{
  // Initialize GPIO21 as an output
  pinMode(21, OUTPUT);
  // Start the serial communication
  communication_init();
}

void loop()
{
  uint8_t command;
  size_t len = communication_read(&command, sizeof(command));

  if (len > 0)
  {
    if (command == TOGGLE_RELAY)
    {
      // Toggle the LED state
      int currentState = digitalRead(21);
      int newState = !currentState;
      digitalWrite(21, newState);

      // Send acknowledgment
      uint8_t response = (newState == HIGH) ? LED_ON : LED_OFF;
      communication_write(&response, sizeof(response));
    }
    else
    {
      // Handle other commands or send an error response
    }
  }
}