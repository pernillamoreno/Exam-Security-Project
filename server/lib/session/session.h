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
#include <mbedtls/sha256.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>

enum
{
    SESSION_OKAY,
    SESSION_GET_TEMP,
    SESSION_ESTABLISH,
    SESSION_TOGGLE_RELAY,
    SESSION_ERROR,
    SESSION_WARNING,
    SESSION_CLOSE

};
enum
{
    STATUS_EXPIRED,
    STATUS_HASH_ERROR,
    STATUS_BAD_REQUEST,
    STATUS_INVALID_SESSION,
    STATUS_CONNECTION_ERROR
};

// Function Prototypes

static size_t client_read(uint8_t *buf, size_t blen);        // First, read the data, blen= max lenght of buffer
static bool client_write(const uint8_t *buf, size_t dlen);   // Then, write data, dlen = lenght of data in he buffer
static void exchange_public_keys(void);                      // Exchange keys after reading and writing
static bool session_write(const uint8_t *data, size_t size); // Write session-specific data, size = size of data to be written
bool session_init(void);
                                         // Initialize the session (HMAC, AES, etc.)
bool session_establish(void);
int session_request(void);                                      // Establish the session
bool session_response(const uint8_t *res, size_t rlen);     // Send responses, rlen = len of response 
void session_close(void);                                        // Finally, close the session

#endif /* SESSION_H */
