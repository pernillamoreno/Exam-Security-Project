#ifndef SESSION_H
#define SESSION_H

#include <stdint.h>
#include <stddef.h>

typedef enum
{
    SESSION_CLOSE,
    SESSION_ERROR,
    SESSION_TOGGLE_LED,
    SESSION_GET_TEMP,

    SESSION_ESTABLISH,
} request_t;

#endif 