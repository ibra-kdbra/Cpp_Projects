#include <iostream>
#include <string.h>

#include <cstdio>

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include "utility.h"

#define HELP_TEXT "Usage: ./ufsend <input file> [-d <IP:Port>] [-l]"
#define MD_SIZE 32
#define MAX_INPUT_FILE_SIZE INT32_MAX - MD_SIZE

using namespace std;

void encrypt_and_send_or_save(string filename, bool save_local, string receiver);

int main(int argc, char *argv[])
{

    bool save_locally = false;
    string receiver;
    string filename;
    string password;

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
            save_locally = true;
        }
        else if (!strcmp(argv[i], "-d"))
        {
            // Get IP:Port after the destination switch
            i++;
            if (i >= argc)
            {
                cerr << HELP_TEXT << endl;
                return 1;
            }
            else
            {
                receiver = string(argv[i]);
            }
        }
        // Assign filename only once at the start
        else if (filename.empty())
        {
            filename = string(argv[i]);
        }
    }
    // Check if whether no filename was provided
    if (filename.empty())
    {
        cerr << "No file was specified." << endl;
        cerr << HELP_TEXT << endl;
        exit(1);
    }

    // Check if the filename with the .ufsec appended is already present
    if (save_locally)
    {
        if (is_file_present(filename + string(".ufsec")))
        {
            cerr << "File " << (filename + string(".ufsec")).c_str() << " already exists, quiting..." << endl;
            exit(33);
        }
    }

    // Encrypt and send (to receiver) or save (with .ufsec appended) the file
    encrypt_and_send_or_save(filename, save_locally, receiver);

    return 0;
}

void encrypt_and_send_or_save(string filename, bool save_local, string receiver)
{
    // Open the input file
    FILE *input_fp = fopen(filename.c_str(), "rb");

    // Check if the file opened
    if (!input_fp)
    {
        cerr << "Input file " << filename << " could not be opened." << endl;
        exit(1);
    }

    // Check if the file size is under the locally defined MAX_FILE_SIZE
    fseek(input_fp, 0, SEEK_END);
    long input_sz = ftell(input_fp);
    if (input_sz > MAX_INPUT_FILE_SIZE)
    {
        cerr << "Input file " << filename << " is too big. Refusing to accept file greater than " << MAX_INPUT_FILE_SIZE << " bytes." << endl;
        fclose(input_fp);
        exit(1);
    }
    fseek(input_fp, 0, SEEK_SET);

    string password;
    unsigned char derived_key[32];
    unsigned char iv[12];

    cout << "Password: ";
    cin >> password;

    // Derive the AES key using the imported common utility function
    derive_key_using_pbkdf2(password, derived_key, sizeof(derived_key));

    cout << "Key: ";
    cout << uppercase << hex;
    for (auto i = 0; i < sizeof(derived_key); i++)
    {
        cout << (int)derived_key[i] << " ";
    }
    cout << endl;
    cout << nouppercase << dec;

    // Fill the IV with random bytes generated using a cryptographically secure pseudo random generator
    RAND_bytes(iv, sizeof(iv));

    BIO *input_bio = BIO_new_fp(input_fp, BIO_CLOSE);

    // Create a digest BIO to check for file integrity on receiver
    BIO *digest_bio = BIO_new(BIO_f_md());
    BIO_set_md(digest_bio, EVP_sha3_256());

    // Chain the digest BIO with the input BIO
    BIO *input_digest_bio = BIO_push(digest_bio, input_bio);

    unsigned char *buff = new unsigned char[MD_SIZE + input_sz];
    unsigned char digest[MD_SIZE];

    // Read the input file
    int readCharCount = BIO_read(input_digest_bio, buff, input_sz);

    // Get the plaintext digest from the chained digest BIO
    BIO_gets(digest_bio, (char *)digest, MD_SIZE);

    BIO_free_all(input_digest_bio);

    if (readCharCount <= 0)
    {
        cerr << "No input was read, quitting." << endl;
    }

    // Copy the digest of the plaintext into buffer to send for encryption
    memcpy(buff + input_sz, digest, MD_SIZE);
    int cipher_size = readCharCount + MD_SIZE;

    // Create a Cipher Filter BIO with AES-256 with GCM
    BIO *cipher_bio = BIO_new(BIO_f_cipher());
    BIO_set_cipher(cipher_bio, EVP_aes_256_gcm(), derived_key, iv, 1);

    // Create a Memory BIO to buffer the output of cipher
    BIO *mem_bio = BIO_new(BIO_s_mem());
    BIO_set_buffer_size(mem_bio, input_sz);

    // Chain the cipher BIO and the memory BIO
    BIO_push(cipher_bio, mem_bio);

    // Send plaintext for encryption
    BIO_write(cipher_bio, buff, cipher_size);

    // Read the ciphertext
    BIO_read(mem_bio, buff, cipher_size);

    BIO_free(cipher_bio);

    cout << "Ciphertext" << endl;
    cout << hex << uppercase;
    for (int i = 0; i < cipher_size; i++)
    {
        cout << (int)buff[i] << " ";
    }
    cout << dec << nouppercase;
    cout << endl;

    BIO *out_bio;

    // Set the output BIO to either a file BIO or a socket BIO depend on the save_local switch
    if (save_local)
    {
        filename.append(".ufsec");

        FILE *output_fp = fopen(filename.c_str(), "wb");

        // Check if the we were able to open the output file
        if (!output_fp)
        {
            cerr << "Output file " << filename << " could not be opened." << endl;
            exit(1);
        }
        cout << "Writing to " << filename.c_str() << endl;
        out_bio = BIO_new_fp(output_fp, BIO_CLOSE);
    }
    else
    {
        out_bio = BIO_new_connect(receiver.c_str());
        if (BIO_do_connect(out_bio) <= 0)
        {
            cerr << "Cannot connect to host: " << receiver << ". Please check if the host is reachable." << endl;
            exit(1);
        }
        cout << "Connected to host " << receiver << ". Transmitting..." << endl;
    }

    // Write IV|Cipher size|Ciphertext to the output BIO
    BIO_write(out_bio, iv, sizeof(iv));
    BIO_write(out_bio, (void *)(&cipher_size), sizeof(int));
    BIO_write(out_bio, buff, cipher_size);

    BIO_free(out_bio);
    delete[] buff;

    if (save_local)
    {
        cout << "Finished Writing " << (long)cipher_size + sizeof(int) + sizeof(iv) << " bytes (including ciphertext size and iv) to file " << filename << "." << endl;
    }
    else
    {
        cout << "Finished Sending " << (long)cipher_size + sizeof(int) + sizeof(iv) << " bytes (including ciphertext size and iv)." << endl;
    }
}