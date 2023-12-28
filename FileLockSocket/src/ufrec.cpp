#include <iostream>
#include <string.h>

#include <cstdio>

#include <openssl/evp.h>
#include <openssl/bio.h>

#include "utility.h"

#define HELP_TEXT "Usage: ./ufrec <input file> [-d <Port>] [-l]"
#define MD_SIZE 32

using namespace std;

void read_or_receive_and_decrypt(string port, string filename, bool read_local);

int main(int argc, char *argv[])
{

    bool read_locally = false;
    string listening_port;
    string filename;

    if (argc <= 2 || argc > 5)
    {
        cerr << HELP_TEXT << endl;
        return 1;
    }

    // Parse arguments
    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-l"))
        {
            read_locally = true;
        }
        else if (!strcmp(argv[i], "-d"))
        {
            // Get Port after the destination switch
            i++;
            if (i >= argc)
            {
                cerr << HELP_TEXT << endl;
                return 1;
            }
            else
            {
                listening_port = string(argv[i]);
            }
        }
        // Assign filename only once
        else if (filename.empty())
        {
            filename = string(argv[i]);
        }
    }

    // Remove .ufsec from filename when run in local mode, assuming the string ends with .ufsec
    if (read_locally)
    {
        filename = filename.substr(0, filename.length() - string(".ufsec").length());
    }

    // Check whether no filename was provided
    if (filename.empty())
    {
        cerr << "No file was specified." << endl;
        cerr << HELP_TEXT << endl;
        exit(1);
    }

    // Check if the output file is already present
    if (is_file_present(filename))
    {
        cerr << "File " << filename.c_str() << " already exists, quiting..." << endl;
        exit(33);
    }

    // Receive (on port) or read the file (with .ufsec appended) and decrypt
    read_or_receive_and_decrypt(listening_port, filename, read_locally);

    return 0;
}

void read_or_receive_and_decrypt(string port, string filename, bool read_local)
{
    // Create an input BIO according the read_local switch
    BIO *input_bio;
    if (read_local)
    {
        string encrypted_filename = filename + string(".ufsec");
        FILE *input_fp = fopen(encrypted_filename.c_str(), "rb");
        if (!input_fp)
        {
            cerr << "Input file " << encrypted_filename << " could not be opened." << endl;
            exit(1);
        }
        cout << "Reading encrypted file " << encrypted_filename << endl;

        input_bio = BIO_new_fp(input_fp, BIO_CLOSE);
    }
    else
    {
        BIO *conn_bio = BIO_new_accept(port.c_str());

        if (BIO_do_accept(conn_bio) <= 0)
        {
            cerr << "Cannot listen on port: " << port << "." << endl;
            exit(1);
        }
        cout << "Waiting for connection..." << endl;

        if (BIO_do_accept(conn_bio) <= 0)
        {
            cerr << "Cannot accept the connection: " << port << "." << endl;
            exit(1);
        }
        cout << "Inbound file." << endl;
        input_bio = BIO_pop(conn_bio);
        BIO_free(conn_bio);
    }

    string password;
    unsigned char derived_key[32];
    unsigned char iv[12];

    cout << "Password: ";
    cin >> password;

    // Derive the aes key using the imported common utility function
    derive_key_using_pbkdf2(password, derived_key, sizeof(derived_key));

    cout << "Key: ";
    cout << uppercase << hex;
    for (auto i = 0; i < sizeof(derived_key); i++)
    {
        cout << (int)derived_key[i] << " ";
    }
    cout << endl;
    cout << nouppercase << dec;

    // Read the first 96 bits for the IV
    BIO_read(input_bio, iv, sizeof(iv));

    int cipher_size;
    // Read the ciphertext size (including the digest)
    BIO_read(input_bio, &cipher_size, sizeof(int));

    // The cipher size couldn't be less than or equal to the appended digest
    if(cipher_size <= MD_SIZE) {
        cout<<"Integrity check failed!"<<endl;
        BIO_free(input_bio);
        exit(1);
    }

    unsigned char *buff = new unsigned char[cipher_size];

    // Read the input
    int readCharCount = BIO_read(input_bio, buff, cipher_size);
    BIO_free(input_bio);
    if (readCharCount <= 0)
        cerr << "Input couldn't be read, quitting." << endl;

    // Write the encrypted text to console
    cout << "Ciphertext" << endl;
    cout << hex << uppercase;
    for (int i = 0; i < readCharCount; i++)
    {
        cout << (int)buff[i] << " ";
    }
    cout << endl;
    cout << dec << nouppercase;

    // Create a Cipher Filter BIO with AES-256 with GCM
    BIO *cipher_bio = BIO_new(BIO_f_cipher());
    BIO_set_cipher(cipher_bio, EVP_aes_256_gcm(), derived_key, iv, 1);

    // Create a Memory BIO to save the output of the Cipher BIO
    BIO *mem_bio = BIO_new(BIO_s_mem());
    BIO_set_buffer_size(mem_bio, cipher_size);

    // Chain the cipher BIO and the output bio
    BIO_push(cipher_bio, mem_bio);

    // Write to Cipher Bio
    BIO_write(cipher_bio, buff, readCharCount);

    // Read the whole plaintext (including the digest) from the memory BIO
    BIO_read(mem_bio, buff, readCharCount);

    BIO_free_all(cipher_bio);

    // Create a digest BIO with SHA3 256 for plaintext verification
    BIO *digest_bio = BIO_new(BIO_f_md());
    BIO_set_md(digest_bio, EVP_sha3_256());

    // Create a null BIO to act as the sink for the digest BIO
    BIO *null_bio = BIO_new(BIO_s_null());

    // Chain the digest and null BIO
    BIO *digest_chain_bio = BIO_push(digest_bio, null_bio);

    // Write on the digest chain the plaintext (excluding the digest)
    BIO_write(digest_chain_bio, buff, cipher_size - MD_SIZE);

    unsigned char digest[MD_SIZE];

    // Store the newly calculated digest in digest array
    BIO_gets(digest_bio, (char *)digest, MD_SIZE);

    BIO_free_all(digest_chain_bio);

    // Compare the digests and exit on failure
    for (int i = 0; i < MD_SIZE; i++)
    {
        if (digest[i] != buff[cipher_size - MD_SIZE + i])
        {
            cout << "Integrity check failed! Might be due to incorrect password, quitting." << endl;
            exit(1);
        }
    }

    // Open the output file
    FILE *output_fp = fopen(filename.c_str(), "wb");

    // Check if the we were able to open the output file
    if (!output_fp)
    {
        cerr << "Output file " << filename << " could not be opened." << endl;
        exit(1);
    }

    BIO *output_bio = BIO_new_fp(output_fp, BIO_CLOSE);

    // Write the content of the plaintext (cutting out the appended digest)
    BIO_write(output_bio, buff, cipher_size - MD_SIZE);

    cout << nouppercase << dec;
    cout << "Finished Writing to file " << filename << endl;

    BIO_free(output_bio);
}