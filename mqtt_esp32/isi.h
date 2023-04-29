
#define KAT_SUCCESS          0
#define KAT_FILE_OPEN_ERROR -1
#define KAT_DATA_ERROR      -3
#define KAT_CRYPTO_FAILURE  -4

#define MAX_MESSAGE_LENGTH         32
#define MAX_ASSOCIATED_DATA_LENGTH 32
#define MAX_KEY_LENGTH             32
#define MAX_NONCE_LENGTH           24


void init_buffer(unsigned char *buffer, unsigned long long numbytes);

void padMessage(unsigned char* message, int length);
void fprint_bstr(const char *label, const unsigned char *data, unsigned long long length);
void fprint_bstrq(const char *label, const unsigned char *data, unsigned long long length);

int generate_test_vectors(unsigned char *key, unsigned char *nonce, unsigned char *msg, unsigned char *ad, unsigned long long mlen, unsigned long long adlen);
