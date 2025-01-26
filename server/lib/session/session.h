
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
    SESSION_CLOSE,
    SESSION_GET_TEMP,
    SESSION_ESTABLISH,
    SESSION_TOGGLE_RELAY,
    SESSION_OKAY,
    SESSION_ERROR,
    SESSION_WARNING
};

enum
{
    STATUS_EXPIRED,
    STATUS_HASH_ERROR,
    STATUS_BAD_REQUEST,
    STATUS_INVALID_SESSION,
    STATUS_CONNECTION_ERROR
};

// Constants for Cryptographic Parameters
static constexpr int AES_SIZE = 32;
static constexpr int DER_SIZE = 294;
static constexpr int RSA_SIZE = 256;
static constexpr int HASH_SIZE = 32;
static constexpr int EXPONENT = 65537;
static constexpr int KEEP_ALIVE = 60000;
static constexpr int AES_BLOCK_SIZE = 16;

// Static Cryptographic Contexts
static mbedtls_aes_context aes_ctx;      
static mbedtls_md_context_t hmac_ctx;     
static mbedtls_pk_context client_key_ctx; 
static mbedtls_pk_context server_key_ctx; 
static mbedtls_entropy_context entropy;   
static mbedtls_ctr_drbg_context ctr_drbg; 
static mbedtls_sha256_context sha256_ctx;

// Function Prototypes

static size_t client_read(uint8_t *data, size_t len);        // First, read the data, blen= max lenght of buffer
static bool client_write(const uint8_t *data, size_t dlen);  // Then, write data, dlen = lenght of data in he buffer
static int exchange_keys();                                  // Exchange keys after reading and writing
static bool session_write(const uint8_t *data, size_t size); // Write session-specific data, size = size of data to be written
bool session_init(void);                                     // Initialize the session (HMAC, AES, etc.)
bool session_establish(void);

int session_close(); // Finally, close the session
int session_send_temperature(float temp);
int session_send_relay_state(uint8_t state);
#endif /* SESSION_H */
