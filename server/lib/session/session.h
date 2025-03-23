
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
    STATUS_OKAY,
    STATUS_ERROR,
    STATUS_EXPIRED
};

bool session_init(void); // Initialize the session (HMAC, AES, etc.)
int session_establish(void);
int session_request(void);
int session_close(); // Finally, close the session
int session_send_error(void);
int session_send_temperature(float temp);
int session_send_relay_state(uint8_t state);

#endif /* SESSION_H */
