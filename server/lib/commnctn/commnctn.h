#ifndef COMMNCTN_H
#define COMMNCTN_H

#include <stdint.h>
#include <stddef.h>

bool communication_init(void);
bool communication_send(const uint8_t *data, size_t dlen);
size_t communication_receive(uint8_t *buf, size_t blen);

#endif /* COMMNCTN_H */