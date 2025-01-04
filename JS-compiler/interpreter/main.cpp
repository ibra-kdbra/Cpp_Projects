#include "interpreter.h"

#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#define F_SIZE 1024 * 16


int main(int argc, char **argv) 
{
    if (argc != 2) {
        printf("usage: ./a.out file\n");
        return -1;
    }

    char *buf = (char *)malloc(sizeof(char) * F_SIZE);
    assert(buf != NULL);

    int fd = open(argv[1], O_RDONLY);
    assert(fd > 0);
    read(fd, buf, F_SIZE);

    simple_lexser::Lexser lexser;
    lexser.lex(buf);

    simple_parser::Parser parser(lexser);
    parser.parse();

    simple_interpreter::Interpreter interpreter(parser);
    interpreter.interprete();

    free(buf);
    return 0;
}