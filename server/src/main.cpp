#include "communication.h"
#include "session.h"
#include <Arduino.h>

// Define macros
#define TOGGLE_RELAY 0x03
#define GET_TEMP 0x01
#define LED_ON 0x01
#define LED_OFF 0x00

#define PIN_32 32 // Relay control pin
#define PIN_21 2  // ESP32 inbuilt LED

void setup()
{
  pinMode(PIN_32, OUTPUT); // Set Pin 32 as an output (relay)
  pinMode(PIN_21, OUTPUT); // Set Pin 2 as an output (for debugging)
  communication_init();    // Initialize serial communication
  Serial.setTimeout(50);
}
void loop()
{
  uint8_t command = 0;
  size_t len = Serial.readBytes((char *)&command, 1); // Read 1 byte with timeout

  if (len > 0) // Only process if data was received
  {
    if (command == TOGGLE_RELAY)
    {
      int currentState = digitalRead(PIN_32);
      digitalWrite(PIN_32, !currentState);

      uint8_t response = (!currentState) ? LED_ON : LED_OFF;
      communication_write(&response, sizeof(response));
    }
    else if (command == GET_TEMP)
    {
      float temperature = temperatureRead();
      char tempBuffer[10];
      dtostrf(temperature, 6, 2, tempBuffer);
      communication_write((uint8_t *)tempBuffer, strlen(tempBuffer) + 1);
    }
  }
}
