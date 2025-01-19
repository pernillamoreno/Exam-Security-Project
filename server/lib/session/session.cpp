/**
 * @file session.cpp
 * @author Pernilla S S-Moreno
 * @brief Secure session implementation with HMAC and AES encryption.
 * @version 0.1
 * @date 2025-01-09
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "session.h"
#include "commnctn.h"
#include <mbedtls/md.h>
#include <mbedtls/pk.h>
#include <mbedtls/rsa.h>
#include <mbedtls/aes.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <Arduino.h>

/**
 * @brief Reads data from communication and verifies integrity using HMAC.
 *
 * @param buf Buffer to store received data.
 * @param blen Maximum length of the buffer.
 * @return Length of valid data if successful, otherwise 0.
 */
size_t Session::client_receive(uint8_t *buf, size_t blen)
{
    size_t length = communication_receive(buf, blen);

    if (length > HASH_SIZE)
    {
        length -= HASH_SIZE;
        uint8_t hmac[HASH_SIZE]{0};
        mbedtls_md_hmac_starts(&hmac_ctx, secret_key, HASH_SIZE);
        mbedtls_md_hmac_update(&hmac_ctx, buf, length);
        mbedtls_md_hmac_finish(&hmac_ctx, hmac);
        if (memcmp(hmac, buf + length, HASH_SIZE) != 0)
        {
            length = 0; // Data integrity failed
        }
    }
    else
    {
        length = 0; // Invalid message length
    }

    return length;
}

/**
 * @brief Writes data with HMAC integrity check.
 *
 * @param buf Pointer to data buffer.
 * @param dlen Length of data.
 * @return True if data is sent successfully, false otherwise.
 */
bool Session::client_send(uint8_t *buf, size_t dlen)
{
    mbedtls_md_hmac_starts(&hmac_ctx, secret_key, HASH_SIZE);
    mbedtls_md_hmac_update(&hmac_ctx, buf, dlen);
    mbedtls_md_hmac_finish(&hmac_ctx, buf + dlen);
    dlen += HASH_SIZE;

    return communication_send(buf, dlen);
}

/**
 * @brief Handles public key exchange between client and server.
 */
void Session::exchange_public_keys(void)
{
    session_id = 0;
    size_t olen, length;
    bool status = false;
}

/**
 * @brief Writes data securely using AES encryption.
 *
 * @param data Pointer to data.
 * @param size Size of the data.
 * @return True if write is successful, false otherwise.
 */
bool Session::session_write(const uint8_t *data, size_t size)
{
    bool status = false;
    return status;
}

/**
 * @brief Initializes the session.
 *
 * @return True if successful, false otherwise.
 */
bool Session::session_init(void)
{
    bool status = false;
    return status;
}

/**
 * @brief Establishes a secure session.
 *
 * @return True if successful, false otherwise.
 */
bool Session::session_establish(void)
{
    return 0;
}

/**
 * @brief Processes incoming session requests.
 *
 * @return Request type.
 */
int Session::session_request(void)
{
    return 0;
}

/**
 * @brief Sends a response over the session.
 *
 * @param res Response data.
 * @param rlen Length of response data.
 * @return True if response is sent successfully, false otherwise.
 */
bool Session::session_response(const uint8_t *res, size_t rlen)
{
    size_t len = 1;
    uint8_t response[AES_BLOCK_SIZE] = {0};
    return session_write(response, len);
}

/**
 * @brief Closes the session.
 */
void Session::session_close(void)
{
    session_id = 0;
}

/**
 * @brief Sends the temperature value over the session.
 *
 * @param temp Temperature value.
 * @return SESSION_OKAY if successful, otherwise SESSION_ERROR.
 */
int Session::session_send_temperature(float temp)
{
    uint8_t temp_buf[sizeof(float)];
    memcpy(temp_buf, &temp, sizeof(float));
    return client_send(temp_buf, sizeof(temp_buf)) ? SESSION_OKAY : SESSION_ERROR;
}

/**
 * @brief Sends the relay state over the session.
 *
 * @param state Relay state (ON/OFF).
 * @return SESSION_OKAY if successful, otherwise SESSION_ERROR.
 */
int Session::session_send_relay_state(uint8_t state)
{
    return client_send(&state, sizeof(state)) ? SESSION_OKAY : SESSION_ERROR;
}
