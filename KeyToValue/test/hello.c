#include "../src/include/k2v.h"
int main(void)
{
	// Get buffer size.
	size_t filesize = k2v_get_filesize("./test/hello.conf");
	// Read the config to memory.
	char *buf = k2v_open_file("./test/hello.conf", filesize);
	// Get the value of `string`.
	char *str = key_get_char("string", buf);
	// Print the value.
	printf("%s\n", str);
	// libk2v will automatically allocate memory.
	// Do not forget to free(2) the memory after using it.
	free(buf);
	free(str);
}
