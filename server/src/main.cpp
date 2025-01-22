#include "commnctn.h"
#include "session.h"
#include <Arduino.h>

// Define macros
#define TOGGLE_RELAY 0x03
#define GET_TEMP 0x01
#define LED_ON 0x01
#define LED_OFF 0x00

// Define pin numbers
#define PIN_32 32 // Relay control pin
#define PIN_21 2  // ESP32 inbuilt LED

void setup()
{
  pinMode(PIN_32, OUTPUT); // Set Pin 32 as an output (relay)
  pinMode(PIN_21, OUTPUT); // Set Pin 2 as an output (for debugging)
  communication_init();    // Initialize serial communication
}

void loop()
{
  uint8_t command;
  size_t len = communication_read(&command, sizeof(command));

  if (len > 0)
  {
    if (command == TOGGLE_RELAY)
    {
      // Toggle relay state on Pin 32
      int currentState = digitalRead(PIN_32);
      int newState = !currentState;
      digitalWrite(PIN_32, newState);

      // Toggle PIN_21 (for debugging)
      int currentState2 = digitalRead(PIN_21);
      int newState2 = !currentState2;
      digitalWrite(PIN_21, newState2);

      // Send acknowledgment
      uint8_t response = (newState == HIGH) ? LED_ON : LED_OFF;
      communication_write(&response, sizeof(response));
    }
    else if (command == GET_TEMP)
    {
      // Use the correct built-in ESP32 function
      float temperature = temperatureRead(); // Get temperature in Celsius
      char tempBuffer[10];
      dtostrf(temperature, 6, 2, tempBuffer); // Convert float to string

      // Send temperature as a string
      communication_write((uint8_t *)tempBuffer, strlen(tempBuffer) + 1);
    }
  }
}
