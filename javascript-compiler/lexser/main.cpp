
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "lexser.h"
#include <assert.h>
#include "token.h"
#define F_SIZE 1024 * 16

int32_t main(int argc, char **argv)
{    
    if (argc != 2) {
        printf("Useage: ./a.out <c_file>");
        exit(-1);
    }

    char *buf = (char *)malloc(sizeof(char) * F_SIZE);
    assert(buf != NULL);

    int fd = open(argv[1], O_RDONLY);
    assert(fd > 0);
    read(fd, buf, F_SIZE);

    simple_lexser::Lexser lexser;
    lexser.lex(buf);
    
    for (std::vector<simple_lexser::Token_t>::iterator it = lexser.token_list.begin();
    it != lexser.token_list.end(); ++it) {
        it->to_string();
    }
    free(buf);
    return 0;
}
