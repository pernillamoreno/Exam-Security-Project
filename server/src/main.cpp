#include <Arduino.h>
#include "session.h"

// Pin definitions
constexpr int RELAY_PIN = GPIO_NUM_32;
constexpr int LED_PIN = GPIO_NUM_21;

static void set_error(bool error)
{
  while (error)
  {
    digitalWrite(LED_PIN, LOW);
    delay(500);
    digitalWrite(LED_PIN, HIGH);
    delay(500);
  }

  digitalWrite(LED_PIN, LOW);
}

void setup()
{
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  if (!session_init())
  {
    set_error(true);
  }
}

void loop()
{
  int request = session_request();

  switch (request)
  {
  case SESSION_ESTABLISH:
    request = session_establish();
    break;

  case SESSION_CLOSE:
    request = session_close();
    break;

  case SESSION_GET_TEMP:
    request = session_send_temperature(temperatureRead());
    break;

  case SESSION_TOGGLE_RELAY:
  {
    static uint8_t state = LOW;
    state = (state == LOW) ? HIGH : LOW;
    digitalWrite(RELAY_PIN, state);
    request = (state == digitalRead(RELAY_PIN)) ? session_send_relay_state(state) : session_send_error();
  }
  break;
  default:
    break;
  }

  set_error(request == SESSION_ERROR);
}
