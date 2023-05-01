void init_buffer(unsigned char *buffer, unsigned long long numbytes);

//void padMessage(unsigned char* message, int length);
void fprint_bstr(const char *label, const unsigned char *data, unsigned long long length);
void fprint_bstrq(const char *label, const unsigned char *data, unsigned long long length);

int generate_test_vectors(unsigned char *key, unsigned char *nonce, unsigned char *msg, unsigned char *ad, unsigned long long mlen, unsigned long long adlen);
