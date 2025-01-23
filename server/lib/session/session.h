#ifndef SESSION_H
#define SESSION_H

#include <stdint.h>
#include <stddef.h>
#include <Arduino.h>

#include <mbedtls/md.h>
#include <mbedtls/pk.h>
#include <mbedtls/aes.h>
#include <mbedtls/sha256.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>

class Session
{
public:
    // 🔹 Define session-related commands
    enum Command
    {
        SESSION_CLOSE,
        SESSION_GET_TEMP,
        SESSION_ESTABLISH,
        SESSION_TOGGLE_RELAY,
        SESSION_OKAY,
        SESSION_ERROR,
        SESSION_WARNING
    };

    // 🔹 Define session status codes
    enum Status
    {
        STATUS_OKAY,
        STATUS_ERROR,
        STATUS_EXPIRED,
        STATUS_HASH_ERROR,
        STATUS_BAD_REQUEST,
        STATUS_INVALID_SESSION,
        STATUS_CONNECTION_ERROR
    };

private:
    static constexpr int AES_SIZE = 32;
    static constexpr int DER_SIZE = 294;
    static constexpr int RSA_SIZE = 256;
    static constexpr int HASH_SIZE = 32;
    static constexpr int EXPONENT = 65537;
    static constexpr int AES_BLOCK_SIZE = 16;

    // Crypto Contexts
    mbedtls_aes_context aes_ctx;
    mbedtls_md_context_t hmac_ctx;
    mbedtls_pk_context client_ctx;
    mbedtls_pk_context server_ctx;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;

    // Session Variables
    uint64_t session_id = 0;
    uint8_t aes_key[AES_SIZE] = {0};
    uint8_t enc_iv[AES_BLOCK_SIZE] = {0};
    uint8_t dec_iv[AES_BLOCK_SIZE] = {0};
    uint8_t buffer[DER_SIZE + RSA_SIZE] = {0};

    // Secret Key
    static const uint8_t secret_key[HASH_SIZE];

    // Communication Interface
    Stream *com;

    // Private Methods
    bool send(const uint8_t *data, size_t len);
    size_t receive(uint8_t *data, size_t len);
    int exchange_keys();

public:
    Session(Stream *stream); // Constructor
    int session_init();
    int session_close();
    int session_establish();
    int session_send_temperature(float temp);
    int session_send_relay_state(uint8_t state);
};

#endif
