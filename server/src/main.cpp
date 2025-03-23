#include <Arduino.h>
#include "session.h"

#define RELAY_PIN GPIO_NUM_32

void setup()
{
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(21, OUTPUT);

  if (!session_init())
  {
    while (1)
    {
      digitalWrite(21, HIGH);
      delay(200);
      digitalWrite(21, LOW);
      delay(200);
    }
  }
}

void loop()
{
  int req = session_request();

  switch (req)
  {
  case SESSION_ESTABLISH:
    session_establish();
    break;

  case SESSION_CLOSE:
    session_close();
    break;

  case SESSION_GET_TEMP:
    session_send_temperature(temperatureRead());
    break;

  case SESSION_TOGGLE_RELAY:
  {
    static uint8_t state = LOW;
    state = (state == LOW) ? HIGH : LOW;
    digitalWrite(RELAY_PIN, state);
    session_send_relay_state(state);
    break;
  }

  default:
    break;
  }

  if (req == SESSION_ERROR)
  {
    while (1)
    {
      digitalWrite(21, HIGH);
      delay(200);
      digitalWrite(21, LOW);
      delay(200);
    }
  }
}
