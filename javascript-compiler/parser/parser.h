#pragma once
#include "../lexser/lexser.h"

namespace simple_parser {

class AstToken;

class Parser {
    private:
    int current_lex_index;
    simple_lexser::Lexser lexser;

    public:
    Parser(simple_lexser::Lexser lexser): lexser(lexser), current_lex_index(0),
        ast_tree_root(nullptr) {}

    ~Parser();

    AstToken* ast_tree_root;
    void parse();

    std::string peek_current_value();
    simple_lexser::token_type peek_current_type();
    void advance(const char *expect);
    void advance(void);
    AstToken *get_current_token();
    AstToken *expression(int bp);
    AstToken *statement();
    AstToken *statements();
    AstToken *block();
    AstToken *function();
    AstToken *variable();

    void parse_expression();
    void delete_tree(AstToken* root);
};

}