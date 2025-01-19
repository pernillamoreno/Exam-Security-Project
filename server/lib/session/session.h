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
#include <Arduino.h>
#include <mbedtls/md.h>
#include <mbedtls/pk.h>
#include <mbedtls/rsa.h>
#include <mbedtls/aes.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>

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

class Session
{
private:
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
    static constexpr int SESSION_TIMEOUT = 3000; /**< Session timeout in milliseconds */

    // Static Cryptographic Contexts
    mbedtls_aes_context aes_ctx;       /**< AES Context */
    mbedtls_md_context_t hmac_ctx;     /**< HMAC Context */
    mbedtls_pk_context client_key_ctx; /**< Client Public Key Context */
    mbedtls_pk_context server_key_ctx; /**< Server Public Key Context */
    mbedtls_entropy_context entropy;   /**< Entropy Context */
    mbedtls_ctr_drbg_context ctr_drbg; /**< CTR DRBG Context */

    // Session Variables
    uint32_t accessed = 0;
    uint64_t session_id = 0;
    uint8_t aes_key[AES_SIZE] = {0};
    uint8_t enc_iv[AES_BLOCK_SIZE] = {0};
    uint8_t dec_iv[AES_BLOCK_SIZE] = {0};
    uint8_t buffer[DER_SIZE + RSA_SIZE] = {0};

    // Security key
    const uint8_t secret_key[HASH_SIZE] = {0x29, 0x49, 0xde, 0xc2, 0x3e, 0x1e, 0x34, 0xb5,
                                           0x2d, 0x22, 0xb5, 0xba, 0x4c, 0x34, 0x23, 0x3a,
                                           0x9d, 0x3f, 0xe2, 0x97, 0x14, 0xbe, 0x24, 0x62,
                                           0x81, 0x0c, 0x86, 0xb1, 0xf6, 0x92, 0x54, 0xd6};

    // Function Prototypes

    size_t client_receive(uint8_t *buf, size_t blen);     // First, read the data, blen= max lenght of buffer
    bool client_send(uint8_t *buf, size_t dlen);          // Then, write data, dlen = lenght of data in he buffer
    void exchange_public_keys(void);                      // Exchange keys after reading and writing
    bool session_write(const uint8_t *data, size_t size); // Write session-specific data, size = size of data to be written

public:
    bool session_init(void); // Initialize the session (HMAC, AES, etc.)
    bool session_establish(void);
    int session_request(void);                              // Establish the session
    bool session_response(const uint8_t *res, size_t rlen); // Send responses, rlen = len of response
    void session_close(void);
    int session_send_temperature(float temp);
    int session_send_relay_state(uint8_t state); // Finally, close the session
};
#endif /* SESSION_H */
