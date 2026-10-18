#ifndef SHA256_H
#define SHA256_H

#include <stddef.h>
#include <stdint.h>

#define SHA256_BLOCK_SIZE 32
#define SHA256_HEX_SIZE 65

typedef struct {
  uint8_t data[64];
  uint32_t datalen;
  uint64_t bitlen;
  uint32_t state[8];
} SHA256_CTX;

void sha256_init(SHA256_CTX *ctx);
void sha256_update(SHA256_CTX *ctx, const uint8_t *data, size_t len);
void sha256_final(SHA256_CTX *ctx, uint8_t hash[SHA256_BLOCK_SIZE]);

/* Convenience helper: computes SHA-256 of null-terminated string, outputs
 * 64-char hex string */
void sha256_string(const char *str, char output_hex[SHA256_HEX_SIZE]);

/* Salted password hashing: hash = SHA256(salt + password) */
void hash_password_salted(const char *password, const char *salt,
                          char output_hex[SHA256_HEX_SIZE]);

#endif
