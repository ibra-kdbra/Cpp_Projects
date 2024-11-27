#include "../src/include/k2v.h"
int main(void)
{
	// Get buffer size.
	size_t filesize = k2v_get_filesize("./test/test.conf");
	// Read the file to memory.
	// k2v will automatically call malloc(bufsize),and do basic checks.
	char *buf = k2v_open_file("test/test.conf", filesize);

	// char_val="string"
	char *char_val = key_get_char("char_val", buf);

	// bool_val="true"
	bool bool_val = key_get_bool("bool_val", buf);

	// int_val="114514"
	int int_val = key_get_int("int_val", buf);

	// float_val="19.19810"
	float float_val = key_get_float("float_val", buf);

	// int_array_val=["1","2","3"]
	int int_array_val[15] = { 0 };
	int intlen = key_get_int_array("int_array_val", buf, int_array_val, 15);
	// key_get_int_array will return the lenth of the array.
	// We set the end of int_array_val to 0.
	int_array_val[intlen] = 0;

	// float_array_val=["1.0","2.0","3.0"]
	float float_array_val[15] = { 0 };
	int floatlen = key_get_float_array("float_array_val", buf, float_array_val, 15);
	float_array_val[floatlen] = 0;

	// char_array_val=["string1","string2","string3"]
	char *char_array_val[15] = { NULL };
	key_get_char_array("char_array_val", buf, char_array_val, 15);

	// Null string test.
	if (key_get_char("null_char", buf) == NULL) {
		printf("null char\n");
	}

	// Print the value we get.
	printf("%s\n", char_val);
	free(char_val);
	printf(bool_val ? "true\n" : "false\n");
	printf("%d\n", int_val);
	printf("%f\n", float_val);
	for (int i = 0; true; i++) {
		if (int_array_val[i] == 0) {
			printf("\n");
			break;
		}
		printf("%d ", int_array_val[i]);
	}
	for (int i = 0; true; i++) {
		if (float_array_val[i] == 0) {
			printf("\n");
			break;
		}
		printf("%f ", float_array_val[i]);
	}
	for (int i = 0; true; i++) {
		if (char_array_val[i] == 0) {
			printf("\n");
			break;
		}
		printf("%s ", char_array_val[i]);
		free(char_array_val[i]);
	}

	// have_key test.
	printf(have_key("xxxx", buf) ? "true\n" : "false\n");
	printf(have_key("yyyy", buf) ? "true\n" : "false\n");
	printf(have_key("zzzz", buf) ? "true\n" : "false\n");
	char *new_char_val = key_get_char("char val", buf);
	printf(new_char_val == NULL ? "null\n" : new_char_val);
	free(new_char_val);
	free(buf);
}
