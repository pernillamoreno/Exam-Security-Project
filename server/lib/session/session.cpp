#include "session.h"
#include <Arduino.h>
#include <mbedtls/md.h>
#include <mbedtls/pk.h>
#include <mbedtls/rsa.h>
#include <mbedtls/aes.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include "communication.h"

static constexpr int AES_SIZE = 32;
static constexpr int DER_SIZE = 294;
static constexpr int RSA_SIZE = 256;
static constexpr int HASH_SIZE = 32;
static constexpr int EXPONENT = 65537;
static constexpr int AES_BLOCK_SIZE = 16;
static constexpr uint32_t KEEP_ALIVE = 60000;

static mbedtls_aes_context aes_ctx;
static mbedtls_md_context_t hmac_ctx;
static mbedtls_pk_context client_key_ctx;
static mbedtls_pk_context server_key_ctx;
static mbedtls_entropy_context entropy;
static mbedtls_ctr_drbg_context ctr_drbg;

static uint64_t accessed = 0;
static uint64_t session_id = 0;
static uint8_t aes_key[AES_SIZE] = {0};
static uint8_t enc_iv[AES_BLOCK_SIZE] = {0};
static uint8_t dec_iv[AES_BLOCK_SIZE] = {0};
static uint8_t buffer[DER_SIZE + RSA_SIZE] = {0};

static const uint8_t secret_key[HASH_SIZE] = {0x29, 0x49, 0xde, 0xc2, 0x3e, 0x1e, 0x34, 0xb5, 0x2d, 0x22, 0xb5,
                                              0xba, 0x4c, 0x34, 0x23, 0x3a, 0x9d, 0x3f, 0xe2, 0x97, 0x14, 0xbe,
                                              0x24, 0x62, 0x81, 0x0c, 0x86, 0xb1, 0xf6, 0x92, 0x54, 0xd6};

static size_t client_read(uint8_t *buf, size_t blen)
{
    size_t length = communication_read(buf, blen);

    if (length > HASH_SIZE)
    {
        length -= HASH_SIZE;
        uint8_t hmac[HASH_SIZE]{0};
        mbedtls_md_hmac_starts(&hmac_ctx, secret_key, HASH_SIZE);
        mbedtls_md_hmac_update(&hmac_ctx, buf, length);
        mbedtls_md_hmac_finish(&hmac_ctx, hmac);
        if (0 != memcmp(hmac, buf + length, HASH_SIZE))
        {
            length = 0;
        }
    }
    else
    {
        length = 0;
    }

    return length;
}

static bool client_write(uint8_t *buf, size_t dlen)
{
    mbedtls_md_hmac_starts(&hmac_ctx, secret_key, HASH_SIZE);
    mbedtls_md_hmac_update(&hmac_ctx, buf, dlen);
    mbedtls_md_hmac_finish(&hmac_ctx, buf + dlen);

    return communication_write(buf, dlen + HASH_SIZE);
}

static int exchange_public_keys(void)
{
    size_t len, length;
    int status = SESSION_ERROR;
    uint8_t cipher[3 * RSA_SIZE + HASH_SIZE] = {0};

    session_id = 0;
    mbedtls_pk_init(&client_key_ctx);

    if (0 == mbedtls_pk_parse_public_key(&client_key_ctx, buffer, DER_SIZE))
    {
        if (MBEDTLS_PK_RSA == mbedtls_pk_get_type(&client_key_ctx))
        {
            if (DER_SIZE == mbedtls_pk_write_pubkey_der(&server_key_ctx, buffer, DER_SIZE))
            {
                if (0 == mbedtls_pk_encrypt(&client_key_ctx, buffer, DER_SIZE / 2, cipher, &len, RSA_SIZE, mbedtls_ctr_drbg_random, &ctr_drbg))
                {
                    if (0 == mbedtls_pk_encrypt(&client_key_ctx, buffer + DER_SIZE / 2, DER_SIZE / 2, cipher + RSA_SIZE, &len, RSA_SIZE, mbedtls_ctr_drbg_random, &ctr_drbg))
                    {
                        status = SESSION_OKAY;
                    }
                }
            }
        }
    }

    length = 2 * RSA_SIZE;
    if (!client_write(cipher, length))
    {
        status = SESSION_ERROR;
    }

    if (status == SESSION_OKAY)
    {
        status = SESSION_ERROR;
        length = client_read(cipher, sizeof(cipher));

        if (length == 3 * RSA_SIZE)
        {
            if (0 == mbedtls_pk_decrypt(&server_key_ctx, cipher, RSA_SIZE, buffer, &len, RSA_SIZE, mbedtls_ctr_drbg_random, &ctr_drbg))
            {
                length = len;
                if (0 == mbedtls_pk_decrypt(&server_key_ctx, cipher + RSA_SIZE, RSA_SIZE, buffer + length, &len, RSA_SIZE, mbedtls_ctr_drbg_random, &ctr_drbg))
                {
                    length += len;
                    if (0 == mbedtls_pk_decrypt(&server_key_ctx, cipher + 2 * RSA_SIZE, RSA_SIZE, buffer + length, &len, RSA_SIZE, mbedtls_ctr_drbg_random, &ctr_drbg))
                    {
                        length += len;
                        if (length == (DER_SIZE + RSA_SIZE))
                        {
                            mbedtls_pk_init(&client_key_ctx);
                            if (0 == mbedtls_pk_parse_public_key(&client_key_ctx, buffer, DER_SIZE))
                            {
                                if (MBEDTLS_PK_RSA == mbedtls_pk_get_type(&client_key_ctx))
                                {
                                    if (0 == mbedtls_pk_verify(&client_key_ctx, MBEDTLS_MD_SHA256, secret_key, HASH_SIZE, buffer + DER_SIZE, RSA_SIZE))
                                    {
                                        strcpy((char *)buffer, "DONE");
                                        if (0 == mbedtls_pk_encrypt(&client_key_ctx, buffer, strlen((const char *)buffer), cipher, &len, RSA_SIZE, mbedtls_ctr_drbg_random, &ctr_drbg))
                                        {
                                            status = SESSION_OKAY;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        if (!client_write(cipher, RSA_SIZE))
        {
            status = SESSION_ERROR;
        }
    }

    return status;
}

static int session_write(const uint8_t *res, size_t size)
{
    int status = SESSION_WARNING;
    uint8_t response[AES_BLOCK_SIZE] = {0};
    uint8_t cipher[AES_BLOCK_SIZE + HASH_SIZE] = {0};

    memcpy(response, res, size);

    if (0 == mbedtls_aes_crypt_cbc(&aes_ctx, MBEDTLS_AES_ENCRYPT, sizeof(response), enc_iv, response, cipher))
    {
        status = client_write(cipher, AES_BLOCK_SIZE) ? SESSION_OKAY : SESSION_ERROR;
    }

    return status;
}

bool session_init(void)
{
    bool status = false;

    if (communication_init())
    {
        // Initialize cryptographic contexts
        mbedtls_entropy_init(&entropy);
        mbedtls_ctr_drbg_init(&ctr_drbg);

        for (size_t i = 0; i < sizeof(aes_key); i++)
        {
            aes_key[i] = random(256);
        }

        if (0 == mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, aes_key, sizeof(aes_key)))
        {
            mbedtls_md_init(&hmac_ctx);
            if (0 == mbedtls_md_setup(&hmac_ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 1))
            {
                mbedtls_aes_init(&aes_ctx);
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

int session_request(void)
{
    uint8_t status = STATUS_OKAY;
    int request = SESSION_WARNING;

    size_t length = client_read(buffer, sizeof(buffer));

    if (length == DER_SIZE)
    {
        request = exchange_public_keys();
    }
    else if (length == 2 * RSA_SIZE)
    {
        request = SESSION_ESTABLISH;
    }
    else if (length == AES_BLOCK_SIZE)
    {
        if (session_id != 0)
        {
            uint32_t now = millis();

            if (now - accessed <= KEEP_ALIVE)
            {
                accessed = now;

                uint8_t temp[AES_BLOCK_SIZE]{0};

                if (0 == mbedtls_aes_crypt_cbc(&aes_ctx, MBEDTLS_AES_DECRYPT, AES_BLOCK_SIZE, dec_iv, buffer, temp))
                {
                    if (temp[AES_BLOCK_SIZE - 1] == (sizeof(status) + sizeof(session_id)))
                    {
                        if (0 == memcmp(&session_id, &temp[1], sizeof(session_id)))
                        {
                            switch (temp[0])
                            {
                            case SESSION_CLOSE:
                            case SESSION_GET_TEMP:
                            case SESSION_TOGGLE_RELAY:
                                request = temp[0];
                                break;
                            default:
                                status = STATUS_ERROR;
                                break;
                            }
                        }
                        else
                        {
                            status = STATUS_ERROR;
                        }
                    }
                    else
                    {
                        status = STATUS_ERROR;
                    }
                }
                else
                {
                    status = STATUS_ERROR;
                }
            }
            else
            {
                session_id = 0;
                status = STATUS_EXPIRED;
            }
        }
        else
        {
            status = STATUS_ERROR;
        }
    }
    else
    {
        status = STATUS_ERROR;
    }

    if (request == SESSION_WARNING)
    {
        request = session_write(&status, sizeof(status));

        if (request == SESSION_OKAY)
        {
            request = SESSION_WARNING;
        }
    }

    return request;
}

int session_establish(void)
{
    session_id = 0;
    size_t len, length;
    int status = SESSION_WARNING;
    uint8_t cipher[2 * RSA_SIZE]{0};

    if (0 == mbedtls_pk_decrypt(&server_key_ctx, buffer, RSA_SIZE, cipher, &len, RSA_SIZE, mbedtls_ctr_drbg_random, &ctr_drbg))
    {
        length = len;

        if (0 == mbedtls_pk_decrypt(&server_key_ctx, buffer + RSA_SIZE, RSA_SIZE, cipher + length, &len, RSA_SIZE, mbedtls_ctr_drbg_random, &ctr_drbg))
        {
            length += len;

            if (length == RSA_SIZE)
            {
                if (0 == mbedtls_pk_verify(&client_key_ctx, MBEDTLS_MD_SHA256, secret_key, HASH_SIZE, cipher, RSA_SIZE))
                {
                    while (session_id == 0)
                    {
                        assert(0 == mbedtls_ctr_drbg_random(&ctr_drbg, (uint8_t *)&session_id, sizeof(session_id)));
                    }

                    assert(0 == mbedtls_ctr_drbg_random(&ctr_drbg, enc_iv, sizeof(enc_iv)));
                    memcpy(dec_iv, enc_iv, sizeof(dec_iv));
                    assert(0 == mbedtls_ctr_drbg_random(&ctr_drbg, aes_key, sizeof(aes_key)));

                    if (0 == mbedtls_aes_setkey_enc(&aes_ctx, aes_key, sizeof(aes_key) * CHAR_BIT))
                    {
                        memcpy(buffer, &session_id, sizeof(session_id));
                        length = sizeof(session_id);

                        memcpy(buffer + length, aes_key, sizeof(aes_key));
                        length += sizeof(aes_key);

                        memcpy(buffer + length, enc_iv, sizeof(enc_iv));
                        length += sizeof(enc_iv);

                        status = SESSION_OKAY;
                    }
                    else
                    {
                        session_id = 0;
                    }
                }
            }
        }
    }

    if (status == SESSION_WARNING)
    {
        memset(buffer, 0, sizeof(buffer));
        length = sizeof(session_id) + sizeof(enc_iv) + sizeof(aes_key);
    }

    if (0 == mbedtls_pk_encrypt(&client_key_ctx, buffer, length, cipher, &len, RSA_SIZE, mbedtls_ctr_drbg_random, &ctr_drbg))
    {
        if (!client_write(cipher, RSA_SIZE))
        {
            status = SESSION_ERROR;
        }
    }

    if (status == SESSION_OKAY)
    {
        accessed = millis();
    }
    else
    {
        session_id = 0;
    }

    return status;
}

int session_close(void)
{
    session_id = 0;
    uint8_t status = STATUS_OKAY;
    return session_write(&status, sizeof(status));
}

int session_send_error(void)
{
    uint8_t status = STATUS_ERROR;
    return session_write(&status, sizeof(status));
}

int session_send_temperature(float temp)
{
    uint8_t buf[1 + sizeof(temp)] = {STATUS_OKAY};
    memcpy(buf + 1, &temp, sizeof(temp));
    return session_write(buf, sizeof(buf));
}

int session_send_relay_state(uint8_t state)
{
    uint8_t buf[2] = {STATUS_OKAY, state};
    return session_write(buf, sizeof(buf));
}
