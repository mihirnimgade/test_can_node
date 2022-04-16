#ifndef __XORSHIFT_H__
#define __XORSHIFT_H__

#include "stdint.h"

// Struct declarations

struct xorshift32_state {
    uint32_t a;
};

// Function declarations

uint32_t xorshift32(struct xorshift32_state *state);
uint32_t rand(uint32_t limit);
void rand_array(uint8_t *array, uint8_t length);

#endif
