#include<string.h>

bool is_file_present(std::string filename);
void derive_key_using_pbkdf2(std::string password, unsigned char *derived_key, size_t derived_key_size);
