/**
 * @file commnctn.cpp
 * @author Pernilla S S-Moreno hhh
 * @brief
 * @version 0.1
 * @date 2025-01-09
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "communication.h"
#include <Arduino.h>

#define BAUDRATE 115200

bool communication_init(void)
{
    Serial.begin(BAUDRATE);
    return Serial ? true : false;
}

bool communication_write(const uint8_t *data, size_t len)
{
    return (len == Serial.write(data, len)); /**< Write the data to the Serial Communication */
}

size_t communication_read(uint8_t *buf, size_t blen)
{
    while (0 == Serial.available())
    {
        ;
    }
    return Serial.readBytes(buf, blen);
}
