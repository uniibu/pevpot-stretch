#include <stdio.h>
#include <string.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>

int pbkdf2(const char *pass, const char *salt, long iter, const EVP_MD *digest, unsigned char *out)
{
  unsigned char digtmp[32], itmp[4];
  int k, mdlen, saltlen = strlen(salt);
  unsigned long j;
  HMAC_CTX hctx_tpl, hctx;

  mdlen = EVP_MD_size(digest);
  HMAC_CTX_init(&hctx_tpl);
  HMAC_Init_ex(&hctx_tpl, pass, strlen(pass), digest, NULL);
  itmp[0] = itmp[1] = itmp[2] = 0; itmp[3] = 1;
  HMAC_CTX_copy(&hctx, &hctx_tpl); HMAC_Update(&hctx, (const unsigned char*)salt, saltlen); HMAC_Update(&hctx, itmp, 4); HMAC_Final(&hctx, digtmp, NULL); HMAC_CTX_cleanup(&hctx);
  memcpy(out, digtmp, mdlen);
  for (j = 1; j < iter; j++) {
    HMAC_CTX_copy(&hctx, &hctx_tpl); HMAC_Update(&hctx, digtmp, mdlen); HMAC_Final(&hctx, digtmp, NULL); HMAC_CTX_cleanup(&hctx);
    for (k = 0; k < mdlen; k++) out[k] ^= digtmp[k];
  }
  HMAC_CTX_cleanup(&hctx_tpl);
  return 1;
}

int main(int argc, char* argv[])
{
    size_t i;
    unsigned char out[32];

    if (argc < 2 || argc > 3) {
      printf("Error. Usage: ./stetch hash [iterations]");
      return 1;
    }

    long iterations = 5000000000;
    if (argc == 3)
      iterations = strtol(argv[2], NULL, 10);


    pbkdf2(argv[1], "pevpot", iterations, EVP_sha256(), out);
    for(i=0;i<32;i++) { printf("%02x", out[i]); } printf("\n");
}
