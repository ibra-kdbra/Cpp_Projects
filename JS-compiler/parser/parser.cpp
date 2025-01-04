#include "parser.h"
#include "token_conventor.h"
#include "ast_tree_viewer.h"

namespace simple_parser {

void Parser::advance(const char *expect) {
    simple_lexser::Token_t t = lexser.get_token_by_index(current_lex_index);
    if (t.value == std::string(expect))
        advance();
    else {
        printf("expect %s, got %s at line %d token index %d\n",
            expect, t.value.c_str(), t.line, t.index);
        exit(-1);
    }
}
void Parser::advance(void) {
    ++current_lex_index;
}

std::string Parser::peek_current_value() {
    return lexser.get_token_by_index(current_lex_index).value;
}

simple_lexser::token_type Parser::peek_current_type() {
    return lexser.get_token_by_index(current_lex_index).type;    
}

AstToken *Parser::get_current_token() {
    return TokenConventor::convert(lexser.get_token_by_index(current_lex_index), *this);
}

AstToken *Parser::expression(int bp) {
    AstToken *token = get_current_token();
    advance();
    AstToken *left = token->nud();
    AstToken *current_token;
    for (current_token = get_current_token();
        bp < dynamic_cast<InfixAstToken *>(current_token) ->bp;) {
        token = current_token;
        advance();
        left = token->led(left);
        current_token = get_current_token();
    }
    delete(current_token);
    return left;
}

AstToken *Parser::statement() {
    AstToken *token = get_current_token();
    if ((void *)&AstToken::std != (void *)(token->*(&AstToken::std))) {
        advance();
        return token->std();
    } else {
        delete(token);  // because we will allocate another one in expression()
        token = expression(0);
        advance(";");
        return token;
    }
}

AstToken *Parser::statements() {
    struct simple_parser::TokenMessager messager;
    messager.type = STATEMENTS;
    AstToken *statements_token = new AstToken(messager, *this);
    while (true) {
        if (peek_current_value() == "}" || peek_current_type() == simple_lexser::END) {
            break;
        }
        AstToken *s = statement();
        statements_token->child_list.push_back(s);
    }
    return statements_token;
}

AstToken *Parser::block() {
    AstToken *token = get_current_token();
    advance("{");
    return token->std();
}

AstToken *Parser::function() {
    AstToken *token = get_current_token();
    assert(token->peek_type() == IDENTIFIER);
    token->set_type(FUNCTION);
    advance();
    advance("(");
    while(peek_current_value() != ")") {
        AstToken *variable = get_current_token();
        assert(variable->peek_type() == IDENTIFIER);
        token->child_list.push_back(variable);
        advance();
        if (peek_current_value() == ",")
            advance(",");
    }
    advance(")");
    token->child_list.push_back(block());
    return token;
}

AstToken *Parser::variable() {
    AstToken *token = get_current_token();
    assert(token->peek_type() == IDENTIFIER);

}

void Parser::parse() {
    this->ast_tree_root = statements();
    simple_parser::AstTreeViewer(this->ast_tree_root).view();
}

void Parser::parse_expression() {
    this->ast_tree_root = expression(0);
    simple_parser::AstTreeViewer(this->ast_tree_root).view();
}

Parser::~Parser() {
    delete_tree(ast_tree_root);
}

void Parser::delete_tree(AstToken* root) {
    for (vector<AstToken *>::iterator it = root->child_list.begin();
        it != root->child_list.end();) {
        delete_tree(*it);
        it = root->child_list.erase(it);
    }
    delete(root);
    root = nullptr;
}

}