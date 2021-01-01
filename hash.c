#include "hash.h"

static inline uint64_t
rot(uint64_t x, uint64_t y) {
    return (x << y) | (x >> (64U - y));
}

static void
core(uint64_t s[16])
{
    for (unsigned r = 0; r < 24; r++) {
        for (unsigned i = 0; i < 16; i++) {
            s[15 - i] ^= rot(s[(r + (i + 0)) & 15], r + (i >> 1) + 1)
                      ^  rot(s[(r + (i + 1)) & 15], r + (i << 1) + 1) ^ 1;
        }
    }
}

static inline void
inject(uint64_t *const dst, const unsigned p, const uint64_t val)
{
    dst[p >> 3] ^= val << ((p & 7) << 3);
}

static inline uint8_t
extract(const uint64_t *const src, const unsigned p)
{
    return src[p >> 3] >> ((p & 7) << 3);
}

void
hash_init(struct hash *const h)
{
    *h = (struct hash){0};
}

void
hash_absorb(struct hash *const h,
            const void *const src, const size_t size)
{
    for (size_t i = 0; i < size; i++) {
        inject(h->state, h->pos, ((uint8_t *)src)[i]);
        h->pos++;

        if (h->pos == 32) {
            core(h->state);
            h->pos = 0;
        }
    }
}

void
hash_flip(struct hash *const h)
{
    inject(h->state, h->pos, 0x1F);
    inject(h->state, 32 - 1, 0x80);
    h->pos = 32;
}

void
hash_squeeze(struct hash *const h,
             void *const dst, const size_t size)
{
    for (size_t i = 0; i < size; i++) {
        if (h->pos == 32) {
            core(h->state);
            h->pos = 0;
        }
        ((uint8_t *)dst)[i] = extract(h->state, h->pos);
        h->pos++;
    }
}

#ifdef E72_TEST
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <inttypes.h>

int
main(int argc, char **argv)
{
    struct hash h;
    uint8_t hash[32];
    char *data = argv[1] ? argv[1] : "test";

    clock_t t0 = clock();

    for (int i = 0; i < 10000; i++) {
        hash_init(&h);
        hash_absorb(&h, data, strlen(data));
        hash_flip(&h);
        hash_squeeze(&h, hash, sizeof(hash));
    }
    clock_t t1 = clock();

    printf("time: %.06fs\n", (t1 - t0) / (CLOCKS_PER_SEC * 10000.0));
    printf("e72(\"%s\"): ", data);

    for (int i = 0; i < 32; i++)
        printf("%02" PRIx8, hash[i]);
    printf("\n");

    return 0;
}
#endif

