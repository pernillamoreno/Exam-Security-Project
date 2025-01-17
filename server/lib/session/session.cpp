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

enum
{
    STATUS_OKAY,
    STATUS_ERROR,
    STATUS_EXPIRED,
    STATUS_HASH_ERROR,
    STATUS_BAD_REQUEST,
    STATUS_INVALID_SESSION,
};

// Constants for Cryptographic Parameters
constexpr int AES_SIZE{32};       /**< AES key size (256 bits) */
constexpr int AES_BLOCK_SIZE{16}; /**< AES block size (128 bits) */
constexpr int RSA_SIZE{256};      /**< RSA key size (2048 bits) */
constexpr int DER_SIZE{294};      /**< Maximum DER encoding size */
constexpr int HASH_SIZE{32};      /**< Hash size for HMAC (256 bits) */
constexpr int EXPONENT{65537};
constexpr int SESSION_TIMEOUT{3000}; /**< Session timeout in milliseconds */

// Static Cryptographic Contexts
static mbedtls_aes_context aes_ctx;       /**< AES Context */
static mbedtls_md_context_t hmac_ctx;     /**< HMAC Context */
static mbedtls_pk_context client_key_ctx; /**< Client Public Key Context */
static mbedtls_pk_context server_key_ctx; /**< Server Public Key Context */
static mbedtls_entropy_context entropy;   /**< Entropy Context */
static mbedtls_ctr_drbg_context ctr_drbg; /**< CTR DRBG Context */

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


static size_t client_read(uint8_t *buf, size_t blen)
{
    size_t lenght = communication_read(buf, blen);

    if (lenght > HASH_SIZE)
    {
        lenght -= HASH_SIZE;
        uint8_t hmac[HASH_SIZE]{0};
        mbedtls_md_hmac_starts(&hmac_ctx, secret_key, HASH_SIZE);
        mbedtls_md_hmac_update(&hmac_ctx, buf, lenght);
        mbedtls_md_hmac_finish(&hmac_ctx, hmac);
        if (0 != memcmp(hmac, buf + lenght, HASH_SIZE))
        {
            lenght = 0;
        }
    }
    else
    {
        lenght = 0;
    }

     return lenght;
}

static bool client_write(const uint8_t *buf, size_t dlen)
{
    if (dlen + HASH_SIZE > sizeof(buffer)) // Prevent buffer overflow
    {
        return false;
    }

    uint8_t hmac[HASH_SIZE]; //Store HMAC separately

    //Generate HMAC without modifying `buf`
    mbedtls_md_hmac_starts(&hmac_ctx, secret_key, HASH_SIZE);
    mbedtls_md_hmac_update(&hmac_ctx, buf, dlen);
    mbedtls_md_hmac_finish(&hmac_ctx, hmac); // HMAC is stored in `hmac` array

    // Copy original data + HMAC to `buffer`
    memcpy(buffer, buf, dlen);
    memcpy(buffer + dlen, hmac, HASH_SIZE);

    dlen += HASH_SIZE; //Update length to include HMAC

    return communication_write(buffer, dlen) == dlen; //Ensure full write
}

static void exchange_public_keys(void)
{
    session_id = 0;
    size_t olen, length;
    bool status = false;
}

static bool session_write(const uint8_t *data, size_t size)
{
    bool status = false;
    return status;
} 
bool session_init(void)
{
    bool status = false;
    return status;
}                                     
bool session_establish(void)
{
    return 0;
}
bool session_response(const uint8_t *res, size_t rlen)
{
    size_t len = 1;
    uint8_t response[AES_BLOCK_SIZE] = {0};
    return session_write(response, len);
}

void session_close(void)
{
    session_id = 0;
}
