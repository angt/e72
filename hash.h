#ifndef E72_H__
#define E72_H__

#include <stddef.h>
#include <stdint.h>

struct hash {
    uint64_t state[16];
    unsigned pos;
};

void
hash_init(struct hash *h);

void
hash_absorb(struct hash *h,
            const void *src,
            size_t size);

void
hash_flip(struct hash *h);

void
hash_squeeze(struct hash *h,
             void *dst,
             size_t size);

#endif
