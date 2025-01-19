/**
 * @file commnctn.cpp
 * @author Pernilla S S-Moreno
 * @brief Communication module for ESP32
 * @version 0.1
 * @date 2025-01-09
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "commnctn.h"
#include <Arduino.h>

#define BAUDRATE 115200

/**
 * @brief Initializes serial communication with the defined baud rate.
 *
 * @return true if initialization is successful, false otherwise.
 */
bool communication_init(void)
{
    Serial.begin(BAUDRATE);
    return Serial ? true : false;
}

/**
 * @brief Sends data over serial communication.
 *
 * @param data Pointer to the data buffer to send.
 * @param dlen Length of the data buffer.
 * @return true if all data is written successfully, false otherwise.
 */
bool communication_send(const uint8_t *data, size_t dlen)
{
    return (dlen == Serial.write(data, dlen)); /**< Write data to serial */
}

/**
 * @brief Reads data from serial communication.
 *
 * @param buf Pointer to the buffer where received data will be stored.
 * @param blen Maximum number of bytes to read.
 * @return Number of bytes actually read.
 */
size_t communication_receive(uint8_t *buf, size_t blen)
{
    size_t received = 0;

    while (received < blen)
    {
        if (Serial.available())
        {
            received += Serial.readBytes(buf + received, blen - received);
        }
    }

    return received;
}
