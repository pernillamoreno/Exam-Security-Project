#include "commnctn.h"
#include <Arduino.h>

// Define macros
#define TOGGLE_RELAY 0x03
#define GET_TEMP 0x01
#define LED_ON 0x01
#define LED_OFF 0x00

// Declare ESP32 temperature sensor function
extern "C"
{
  uint8_t temprature_sens_read();
}

void setup()
{
  pinMode(21, OUTPUT);  // Relay pin
  communication_init(); // Initialize communication
}

void loop()
{
  uint8_t command;
  size_t len = communication_read(&command, sizeof(command));

  if (len > 0)
  {
    if (command == TOGGLE_RELAY)
    {
      // Toggle relay state
      int currentState = digitalRead(21);
      int newState = !currentState;
      digitalWrite(21, newState);

      // Send acknowledgment
      uint8_t response = (newState == HIGH) ? LED_ON : LED_OFF;
      communication_write(&response, sizeof(response));
    }
    else if (command == GET_TEMP)
    {
      // Read internal temperature sensor
      uint8_t raw_temperature = temprature_sens_read(); // Raw temp in Fahrenheit
      float temperature = (raw_temperature - 32) / 1.8; // Convert to Celsius

      // Send temperature as a float
      uint8_t buffer[sizeof(float)];
      memcpy(buffer, &temperature, sizeof(float));
      communication_write(buffer, sizeof(buffer));
    }
  }
}
