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

// Constants for Cryptographic Parameters
constexpr int AES_SIZE{32};          /**< AES key size (256 bits) */
constexpr int AES_BLOCK_SIZE{16};    /**< AES block size (128 bits) */
constexpr int RSA_SIZE{256};         /**< RSA key size (2048 bits) */
constexpr int DER_SIZE{294};         /**< Maximum DER encoding size */
constexpr int HASH_SIZE{32};         /**< Hash size for HMAC (256 bits) */
constexpr int EXPONENT{65537};
constexpr int SESSION_TIMEOUT{3000}; /**< Session timeout in milliseconds */

// Session Commands
#define SESSION_CLOSE 0x00
#define SESSION_ESTABLISH 0x01
#define SESSION_GET_TEMP 0x02
#define SESSION_TOGGLE_RELAY 0x03

// Function Prototypes 
static size_t client_read(uint8_t *buffer, size_t blen);      // First, read the data
static bool client_write(const uint8_t *data, size_t dlen);  // Then, write data
static void exchange_public_keys(void);                          // Exchange keys after reading and writing
static bool session_write(const uint8_t *data, size_t dlen); // Write session-specific data
bool session_init(void);
                                         // Initialize the session (HMAC, AES, etc.)
bool session_establish(void);                                    // Establish the session
bool session_response(const uint8_t *data, size_t dlen);     // Send responses
void session_close(void);                                        // Finally, close the session

#endif /* SESSION_H */
