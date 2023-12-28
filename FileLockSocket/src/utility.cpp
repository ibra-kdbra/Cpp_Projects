#include<string.h>
#include<iostream>
#include <openssl/evp.h>
#include <openssl/kdf.h>
#include <openssl/core_names.h>

#define KDF_SALT "SodiumChloride"
#define KDF_ITERATIONS 4096
#define KDF_DIGEST "SHA3-256"

using namespace std;

bool is_file_present(string filename) {
    if (FILE *output_fp = fopen(filename.c_str(), "r"))
    {
        fclose(output_fp);
        return true;
    }
    return false;
}

void derive_key_using_pbkdf2(string password, unsigned char *derived_key, size_t derived_key_size)
{
    // Fetch the PBKDF2 implementation and create its context
    EVP_KDF *pbkdf2 = EVP_KDF_fetch(NULL, "PBKDF2", NULL);
    EVP_KDF_CTX *kdf_ctx = EVP_KDF_CTX_new(pbkdf2);
    EVP_KDF_free(pbkdf2);

    // Construct the parameters for the key derivation
    OSSL_PARAM params[5];
    unsigned int kdf_iterations = (unsigned int)KDF_ITERATIONS;

    params[0] = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_PASSWORD, (void *)&password[0], password.length());
    params[1] = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_SALT, (void *)KDF_SALT, sizeof(KDF_SALT) - 1);
    params[2] = OSSL_PARAM_construct_uint(OSSL_KDF_PARAM_ITER, &kdf_iterations);
    params[3] = OSSL_PARAM_construct_utf8_string(OSSL_KDF_PARAM_DIGEST, (char *)KDF_DIGEST, sizeof(KDF_DIGEST));
    params[4] = OSSL_PARAM_construct_end();

    // Derive the key
    if (EVP_KDF_derive(kdf_ctx, derived_key, derived_key_size, params) <= 0)
    {
        cerr << "Error while deriving key from password";
        exit(1);
    }
}