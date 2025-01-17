/**
 * @file session.h
 * @author Pernilla S S-Moreno
 * @brief 
 * @version 0.1
 * @date 2025-01-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef SESSION_H
#define SESSION_H

#include <stdint.h>
#include <stddef.h>
#include <mbedtls/md.h>
#include <mbedtls/aes.h>
#include <mbedtls/pk.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>


// Session Commands
#define SESSION_CLOSE 0x00
#define SESSION_ESTABLISH 0x01
#define SESSION_GET_TEMP 0x02
#define SESSION_TOGGLE_RELAY 0x03

// Function Prototypes

static size_t client_read(uint8_t *buf, size_t blen);        // First, read the data, blen= max lenght of buffer
static bool client_write(const uint8_t *buf, size_t dlen);   // Then, write data, dlen = lenght of data in he buffer
static void exchange_public_keys(void);                      // Exchange keys after reading and writing
static bool session_write(const uint8_t *data, size_t size); // Write session-specific data, size = size of data to be written
bool session_init(void);
                                         // Initialize the session (HMAC, AES, etc.)
bool session_establish(void);                                    // Establish the session
bool session_response(const uint8_t *res, size_t rlen);     // Send responses, rlen = len of response 
void session_close(void);                                        // Finally, close the session

#endif /* SESSION_H */
