#pragma once
#include "../parser/parser.h"
#include "../parser/ast_token.h"
#include "scope.h"
#include "scope_manager.h"

using namespace simple_parser;
namespace simple_interpreter {

class Interpreter {
    private:
    simple_parser::Parser& parser;
    ScopeManager scopeManager;

    public:
    Interpreter(simple_parser::Parser& p): parser(p) {}

    int expression_resolve(AstToken *exp);
    void statment_resolve(AstToken *root);
    void statments_resolve(AstToken *root);
    void function_resolve(AstToken *root);
    string function_exe(AstToken* root);
    void function_define(AstToken *root);

    void interprete() {
        statments_resolve(parser.ast_tree_root);
    }

    void push_scope() {
        scopeManager.increase_scope_layer();
    }

    void push_scope(string mark) {
        scopeManager.increase_scope_layer(mark);
    }

    void pop_scope() {
        scopeManager.decrease_scope_layer();
    }
};

}