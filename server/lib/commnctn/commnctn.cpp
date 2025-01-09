/**
 * @file commnctn.cpp
 * @author Pernilla S S-Moreno
 * @brief 
 * @version 0.1
 * @date 2025-01-09
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "commnctn.h"
#include <Arduino.h>

#define BAUDRATE 115200

bool communication_init(void)
{
    Serial.begin(BAUDRATE);
    return Serial ? true : false;
    ;
}

bool communication_write(const uint8_t *data, size_t data_len)
{
    return (data_len == Serial.write(data, data_len)); /**< Write the data to the Serial Communication */
}

size_t communication_read(uint8_t *buf, size_t buf_len)
{
    while (0 == Serial.available())
    {
        ; /**< Wait until data is available */
    }

    return Serial.readBytes(buf, buf_len); /**< Read available bytes into the buffer */
}
  