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

static size_t client_read(uint8_t *data, size_t blen)
{
    return communication_read(data, blen) == blen;
}

static bool client_write(const uint8_t *data, size_t dlen)
{

    return communication_write(data, dlen) == dlen; // Ensure full write
}

static int exchange_keys()
{
    return SESSION_OKAY;
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

static bool session_write(const uint8_t *data, size_t size)
{
    bool status = false;
    return status;
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