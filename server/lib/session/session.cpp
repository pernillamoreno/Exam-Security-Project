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
#include "session.h"
#include <Arduino.h>
#include <mbedtls/md.h>
#include <mbedtls/pk.h>
#include <mbedtls/rsa.h>
#include <mbedtls/aes.h>
#include "communication.h"
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>

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
static void exchange_public_keys(void);                      // Exchange keys after reading and writing
static bool session_write(const uint8_t *data, size_t size); // Write session-specific data, size = size of data to be written

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

    uint8_t hmac[HASH_SIZE]; // Store HMAC separately

    // Generate HMAC without modifying `buf`
    mbedtls_md_hmac_starts(&hmac_ctx, secret_key, HASH_SIZE);
    mbedtls_md_hmac_update(&hmac_ctx, buf, dlen);
    mbedtls_md_hmac_finish(&hmac_ctx, hmac); // HMAC is stored in `hmac` array

    // Copy original data + HMAC to `buffer`
    memcpy(buffer, buf, dlen);
    memcpy(buffer + dlen, hmac, HASH_SIZE);

    dlen += HASH_SIZE; // Update length to include HMAC

    return communication_write(buffer, dlen) == dlen; // Ensure full write
}

static void exchange_public_keys(void)
{
    session_id = 0;
    size_t olen, length;

    mbedtls_pk_init(&client_key_ctx);
    uint8_t cipher[3 * RSA_SIZE + HASH_SIZE] = {0};

    assert(0 == mbedtls_pk_parse_public_key(&client_key_ctx, buffer, DER_SIZE));
    assert(MBEDTLS_PK_RSA == mbedtls_pk_get_type(&client_key_ctx));

    assert(DER_SIZE == mbedtls_pk_write_pubkey_der(&server_key_ctx, buffer, DER_SIZE));

    assert(0 == mbedtls_pk_encrypt(&client_key_ctx, buffer, DER_SIZE / 2, cipher,
                                   &olen, RSA_SIZE, mbedtls_ctr_drbg_random, &ctr_drbg));

    assert(0 == mbedtls_pk_encrypt(&client_key_ctx, buffer + DER_SIZE / 2, DER_SIZE / 2,
                                   cipher + RSA_SIZE, &olen, RSA_SIZE, mbedtls_ctr_drbg_random, &ctr_drbg));

    length = 2 * RSA_SIZE;
    assert(client_write(cipher, length));
}

static bool session_write(const uint8_t *res, size_t size)
{
    bool status = false;
    uint8_t response[AES_BLOCK_SIZE] = {0};
    uint8_t cipher[AES_BLOCK_SIZE + HASH_SIZE] = {0};

    memcpy(response, res, size);

    if (0 == mbedtls_aes_crypt_cbc(&aes_ctx, MBEDTLS_AES_ENCRYPT, sizeof(response), enc_iv, response, cipher))
    {
        status = client_write(cipher, AES_BLOCK_SIZE);
    }

    return status;
}

bool session_init(void)
{
    bool status{false};

    if (communication_init())
    {
        // RNG Initialization
        mbedtls_entropy_init(&entropy);
        mbedtls_ctr_drbg_init(&ctr_drbg);
        for (size_t i = 0; i < sizeof(aes_key); i++)
        {
            aes_key[i] = random(256);
        }

        if (0 == mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, aes_key, sizeof(aes_key)))
        {
            // HMAC-SHA256
            mbedtls_md_init(&hmac_ctx);
            if (0 == mbedtls_md_setup(&hmac_ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 1))
            {
                // AES-256
                mbedtls_aes_init(&aes_ctx);

                // RSA-2048
                mbedtls_pk_init(&server_key_ctx);
                if (0 == mbedtls_pk_setup(&server_key_ctx, mbedtls_pk_info_from_type(MBEDTLS_PK_RSA)))
                {
                    status = (0 == mbedtls_rsa_gen_key(mbedtls_pk_rsa(server_key_ctx), mbedtls_ctr_drbg_random, &ctr_drbg, RSA_SIZE * CHAR_BIT, EXPONENT));
                }
            }
        }
    }

    return status;
}

bool session_establish(void)
{
    return 0;
}
bool session_response(bool success, const uint8_t *res, size_t rlen)
{
    size_t len = 1;
    uint8_t response[AES_BLOCK_SIZE] = {0};

    response[0] = success ? STATUS_OKAY : STATUS_ERROR;

    if ((res != nullptr) && (rlen > 0))
    {
        memcpy(response + len, res, rlen);
        len += rlen;
    }

    return session_write(response, len);
}
int session_close()
{
    return 0;
}
int session_send_temperature(float temp)
{
    uint8_t temp_buf[sizeof(float)];
    memcpy(temp_buf, &temp, sizeof(float));
    return client_write(temp_buf, sizeof(temp_buf)) ? SESSION_OKAY : SESSION_ERROR;
}
int session_send_relay_state(uint8_t state)
{
    return client_write(&state, sizeof(state)) ? SESSION_OKAY : SESSION_ERROR;
}