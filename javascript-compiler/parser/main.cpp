#include "parser.h"
#include "../lexser/lexser.h"
#include "ast_tree_viewer.h"

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("usage: ./a.out <exp>\n");
        return -1;
    }

    const char* exp = argv[1];
    simple_lexser::Lexser lexser;
    lexser.lex(exp);

    simple_parser::Parser parser(lexser);
    parser.parse();

    // simple_parser::AstTreeViewer(parser.ast_tree_root).view();

    return 0;
}