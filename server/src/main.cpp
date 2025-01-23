#include "commnctn.h"
#include "session.h"
#include <Arduino.h>

#define SESSION_ESTABLISH 0x01
#define TOGGLE_RELAY 0x03
#define GET_TEMP 0x01
#define LED_ON 0x01
#define LED_OFF 0x00

#define PIN_32 32 // Relay control pin (Relay)
#define PIN_21 2  // ESP32 built-in LED

Session session(&Serial); // Use Serial as communication interface

void setup()
{
  Serial.begin(115200);
  Serial.setRxBufferSize(1024); // Increase RX buffer size
  delay(1000);
  Serial.println("ESP32 Server Starting...");

  pinMode(PIN_32, OUTPUT); // Relay pin
  pinMode(PIN_21, OUTPUT); // Debugging LED pin

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
      // **Toggle LED (Relay)**
      int currentState = digitalRead(PIN_32);
      int newState = !currentState;
      digitalWrite(PIN_32, newState);

      // **Send LED status to client**
      uint8_t response = (newState == HIGH) ? LED_ON : LED_OFF;
      session.session_send_relay_state(response);
    }
    else if (command == GET_TEMP)
    {
      float temperature = temperatureRead(); // Read temperature
      char tempBuffer[10];
      dtostrf(temperature, 6, 2, tempBuffer); // Convert float to string

      Serial.println("Sending Temperature to Client"); // Debugging
      Serial.println(tempBuffer);                      // Debugging

      communication_write((uint8_t *)tempBuffer, strlen(tempBuffer) + 1); // Send temperature string
    }
  }
}