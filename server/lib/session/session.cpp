/**
 * @file session.cpp
 * @author Pernilla S S-Moreno
 * @brief
 * @version 0.1
 * @date 2025-01-09
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "commnctn.h"
#include "session.h"
#include <Arduino.h>
#include <mbedtls/md.h>
#include <mbedtls/pk.h>
#include <mbedtls/rsa.h>
#include <mbedtls/aes.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>

// Constants for Cryptographic Parameters
constexpr int AES_SIZE{32};       /**< AES key size (256 bits) */
constexpr int AES_BLOCK_SIZE{16}; /**< AES block size (128 bits) */
constexpr int RSA_SIZE{256};      /**< RSA key size (2048 bits) */
constexpr int DER_SIZE{294};      /**< Maximum DER encoding size */
constexpr int HASH_SIZE{32};      /**< Hash size for HMAC (256 bits) */
constexpr int EXPONENT{65537};
constexpr int KEEP_ALIVE{60000};
constexpr int SESSION_TIMEOUT{3000}; /**< Session timeout in milliseconds */

// Static Cryptographic Contexts
static mbedtls_aes_context aes_ctx;       /**< AES Context */
static mbedtls_md_context_t hmac_ctx;     /**< HMAC Context */
static mbedtls_pk_context client_key_ctx; /**< Client Public Key Context */
static mbedtls_pk_context server_key_ctx; /**< Server Public Key Context */
static mbedtls_entropy_context entropy;   /**< Entropy Context */
static mbedtls_ctr_drbg_context ctr_drbg; /**< CTR DRBG Context */
static mbedtls_sha256_context sha256_ctx;

// Session Variables
static uint32_t accessed = 0;                     /**< Last time the session was accessed */
static uint64_t session_id = 0;                   /**< Session ID */
static uint8_t aes_key[AES_SIZE] = {0};           /**< AES Key */
static uint8_t enc_iv[AES_BLOCK_SIZE] = {0};      /**< Encryption IV */
static uint8_t dec_iv[AES_BLOCK_SIZE] = {0};      /**< Decryption IV */
static uint8_t buffer[DER_SIZE + RSA_SIZE] = {0}; /**< Temporary Buffer */

// Security key
static const uint8_t secret_key[HASH_SIZE] = {0x29, 0x49, 0xde, 0xc2, 0x3e, 0x1e, 0x34, 0xb5,
                                              0x2d, 0x22, 0xb5, 0xba, 0x4c, 0x34, 0x23, 0x3a,
                                              0x9d, 0x3f, 0xe2, 0x97, 0x14, 0xbe, 0x24, 0x62,
                                              0x81, 0x0c, 0x86, 0xb1, 0xf6, 0x92, 0x54, 0xd6};

// Constructor to initialize communication interface
Session::Session(Stream *stream)
{
    com = stream;
}

// Reads data from the serial communication (previously client_read)
size_t Session::receive(uint8_t *buf, size_t blen)
{
    if (com == nullptr)
        return 0; // Ensure serial interface is set
    return com->readBytes(buf, blen);
}

// Writes data to the serial communication (previously client_write)
bool Session::send(const uint8_t *data, size_t len)
{
    if (com == nullptr)
        return false; // Ensure serial interface is set
    return (com->write(data, len) == len);
}

int Session::exchange_keys()
{

    return SESSION_OKAY;
}
int Session::session_init()
{
    return exchange_keys();
}
int Session::session_close()
{
    return 0;
}
int Session::session_establish()
{
    return 0;
}
int Session::session_send_temperature(float temp)
{
    uint8_t temp_buf[sizeof(float)];
    memcpy(temp_buf, &temp, sizeof(float));
    return send(temp_buf, sizeof(temp_buf)) ? SESSION_OKAY : SESSION_ERROR;
}
int Session::session_send_relay_state(uint8_t state)
{
    return send(&state, sizeof(state)) ? SESSION_OKAY : SESSION_ERROR;
}