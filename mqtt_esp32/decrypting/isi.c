#include "isi.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "api.h"
#include "crypto_aead.h"


void fprint_bstr(const char *label, const unsigned char *data, unsigned long long length);
void fprint_bstrq(const char *label, const unsigned char *data, unsigned long long length) {
    printf("%s: ", label);
    for(unsigned long long i = 0; i < length; i++) {
      printf("%c", data[i]);
    }
    printf("\n");
}

int generate_test_vectors(unsigned char *key, unsigned char *nonce, unsigned char *msg, unsigned char *ad, unsigned long long mlen, unsigned long long adlen)
{
  unsigned char       ct[MAX_MESSAGE_LENGTH + CRYPTO_ABYTES];
  unsigned char       msg2[MAX_MESSAGE_LENGTH];
  unsigned long long  clen, mlen2;
  int                 func_ret, ret_val = KAT_SUCCESS;

  init_buffer(ct, sizeof(ct));

  fprint_bstr("Key = ", key, CRYPTO_KEYBYTES);

  fprint_bstr("Nonce = ", nonce, CRYPTO_NPUBBYTES);

  fprint_bstr("Message = ", msg, 32);


  fprint_bstr("AD = ", ad, 32);

  if ((func_ret = crypto_aead_encrypt(ct, &clen, msg, mlen, ad, adlen, NULL, nonce, key)) != 0) {
    return KAT_CRYPTO_FAILURE;
  }

  fprint_bstr("Encrypted = ", ct, clen);

  if ((func_ret = crypto_aead_decrypt(msg2, &mlen2, NULL, ct, clen, ad, adlen, nonce, key)) != 0) {
    return KAT_CRYPTO_FAILURE;
  }
  fprint_bstr("Decrypted Hex = ", msg2, mlen2);

  fprint_bstrq("Decrypted Text = ", msg2, mlen2);
    
  if (mlen != mlen2) {
    return KAT_CRYPTO_FAILURE;
  }

  if (memcmp(msg, msg2, mlen)) {
    return KAT_CRYPTO_FAILURE;
  }

  return KAT_SUCCESS;
}

void fprint_bstr(const char *label, const unsigned char *data, unsigned long long length) {
    printf("%s: ", label);
    for(unsigned long long i = 0; i < length; i++) {
        printf("%02x ", data[i]);
    }
    printf("\n");
}

void init_buffer(unsigned char *buffer, unsigned long long numbytes)
{
  for (unsigned long long i = 0; i < numbytes; i++)
    buffer[i] = (unsigned char)i;
}

void print_to_csv(char *text){
  FILE* stream = fopen ("datalogger.csv", "w");
  fprintf(stream, "%s", text);
}

int main()
{
//   int ret_val = KAT_SUCCESS;

//   char input_string[MAX_MESSAGE_LENGTH * 2 + 1];
//   unsigned char message[MAX_MESSAGE_LENGTH];
//   int i, input_length;

//   float input_float;
//   printf("Enter float number: ");
//   scanf("%f", &input_float);

//   input_length = snprintf(input_string, MAX_MESSAGE_LENGTH, "%.2f", input_float);

//   printf("Original message:\n%s\n", input_string);

//   if (input_length > MAX_MESSAGE_LENGTH) {
//     // printf("Message too long, truncating to 32 bytes\n");
//     input_length = MAX_MESSAGE_LENGTH;
//   }

//   for (i = 0; i < input_length; i++) {
//     message[i] = (unsigned char) input_string[i];
//   }

//   padMessage(message, input_length);

//   printf("\n");
//   ret_val = generate_test_vectors(key, nonce, message, ad, 32, 32);
//   if (ret_val != KAT_SUCCESS) {
//   printf("Error in generate_test_vectors()\n");
//   return ret_val;
//   }

//   printf("Success!\n");

//   return 0;
}
