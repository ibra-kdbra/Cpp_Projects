#include "interpreter.h"
#include <sstream>

namespace simple_interpreter {

int Interpreter::expression_resolve(AstToken *exp) {
    if (exp->peek_type() == NUM) {
        return std::stoi(exp->peek_value());
    }
    if (exp->peek_type() == IDENTIFIER) {
        return atoi(scopeManager.get_variable(exp->peek_value()).c_str());
    }
    if (exp->peek_value() == "==") {
        assert(exp->child_list.size() == 2);
        return (int)(expression_resolve(exp->child_list[0]) == expression_resolve(exp->child_list[1]));
    }
    if (exp->peek_value() == ">=") {
        assert(exp->child_list.size() == 2);
        return (int)(expression_resolve(exp->child_list[0]) >= expression_resolve(exp->child_list[1]));
    } 
    if (exp->peek_value() == "<=") {
        assert(exp->child_list.size() == 2);
        return (int)(expression_resolve(exp->child_list[0]) <= expression_resolve(exp->child_list[1]));
    }
    if (exp->peek_value() == ">") {
        assert(exp->child_list.size() == 2);
        return (int)(expression_resolve(exp->child_list[0]) > expression_resolve(exp->child_list[1]));
    }
    if (exp->peek_value() == "<") {
        assert(exp->child_list.size() == 2);
        return (int)(expression_resolve(exp->child_list[0]) < expression_resolve(exp->child_list[1]));
    }
    if (exp->peek_value() == "!=") {
        assert(exp->child_list.size() == 2);
        return (int)(expression_resolve(exp->child_list[0]) != expression_resolve(exp->child_list[1]));
    }

    if (exp->peek_value() == "*") {
        assert(exp->child_list.size() == 2);
        return expression_resolve(exp->child_list[0]) * expression_resolve(exp->child_list[1]);
    }
    if (exp->peek_value() == "/") {
        assert(exp->child_list.size() == 2);
        return expression_resolve(exp->child_list[0]) / expression_resolve(exp->child_list[1]);
    }
    if (exp->peek_value() == "%") {
        assert(exp->child_list.size() == 2);
        return expression_resolve(exp->child_list[0]) % expression_resolve(exp->child_list[1]);
    }

    if (exp->peek_value() == "+") {
        assert(exp->child_list.size() <= 2);
        if (exp->child_list.size() == 2)
            return expression_resolve(exp->child_list[0]) + expression_resolve(exp->child_list[1]);
        else
            return expression_resolve(exp->child_list[0]);
    }
    if (exp->peek_value() == "-") {
        assert(exp->child_list.size() <= 2);
        if (exp->child_list.size() == 2)
            return expression_resolve(exp->child_list[0]) - expression_resolve(exp->child_list[1]);
        else
            return -expression_resolve(exp->child_list[0]);
    }

    if (exp->peek_value() == "=") {
        assert(exp->child_list.size() == 2);
        int value = expression_resolve(exp->child_list[1]);
        scopeManager.set_variable(exp->child_list[0]->peek_value(), to_string(value));
        printf("assgin %s %d\n",exp->child_list[0]->peek_value().c_str(), value);
        return value;
    }

    if (exp->peek_value() == "||") {
        assert(exp->child_list.size() == 2);
        return expression_resolve(exp->child_list[0]) ? 1 : 
            (expression_resolve(exp->child_list[1]) ? 1 : 0);
    }

    if (exp->peek_value() == "&&") {
        assert(exp->child_list.size() == 2);
        return expression_resolve(exp->child_list[0]) ? 
            (expression_resolve(exp->child_list[1]) ? 1 : 0) : 0;
    }

    if (exp->peek_type() == FUNCTION) {
        assert(exp->child_list.size() >= 1 && exp->peek_value() == "(");
        return atoi(function_exe(exp).c_str());
    }

    printf("not expression token < %s >, line %d index %d\n", 
        exp->peek_value().c_str(), exp->peek_line(), exp->peek_index());
    exit(-1);
}

void Interpreter::statments_resolve(AstToken *root) {
    assert(root->peek_type() == STATEMENTS);

    // to check whether we should terminate statement_resolve, forexample when return, we
    // should terminate statement_resolve without reach the last statement
    Scope* belong_func_scope = scopeManager.find_last_scope_with_mark("Func");
    string* terminate_addr = nullptr;
    if (belong_func_scope) {  //ok we are within a function
        terminate_addr = belong_func_scope->get_variable("terminate-value");
        assert(terminate_addr != nullptr);
    }

    push_scope();
    for (int i = 0; i < root->child_list.size() && 
        (!!terminate_addr ? (*terminate_addr == "false") : true); ++i) {
        statment_resolve(root->child_list[i]);
    }
    pop_scope();
}

string Interpreter::function_exe(AstToken* root) {
    push_scope("Func");
    string args = scopeManager.get_variable(
        "args-" + to_string(root->child_list.size() - 1) + "-" + 
        root->child_list[0]->peek_value());

    stringstream ss(args);
    string arg;
    int i = 0;
    while (ss >> arg) {
        scopeManager.new_and_set_variable(arg, to_string(expression_resolve(root->child_list[++i])));
    }
    scopeManager.new_variable("ret-value"); // to recieve return value
    scopeManager.new_and_set_variable("terminate-value", "false"); // to recieve terminate status

    // do actual function call
    AstToken *func_address = (AstToken *)stoll(scopeManager.get_variable("func-" + 
        to_string(root->child_list.size() - 1) + "-" + root->child_list[0]->peek_value()));
    statments_resolve(func_address->child_list[func_address->child_list.size() - 1]);
    string ret = scopeManager.get_variable("ret-value");
    pop_scope();
    return ret;
}

void Interpreter::function_define(AstToken *root) {
    assert(root->peek_type() == FUNCTION && root->child_list.size() >= 1);
    //3-fname
    string suffix = to_string(root->child_list.size() - 1) + "-" + root->peek_value();

    //func-3-fname, func_asttoken_address
    scopeManager.new_and_set_variable("func-" + suffix, to_string((uint64_t)root));
    //args-3-fname, 
    string value;
    for (int i = 0; i < root->child_list.size() - 1; ++i) {
        value = value + (root->child_list[i]->peek_value() + " "); 
    }
    scopeManager.new_and_set_variable("args-" + suffix, value);
}

void Interpreter::statment_resolve(AstToken *root) {
    if (root->peek_value() == "if") {
        assert(root->child_list.size() <= 3);
        if (expression_resolve(root->child_list[0])) { // then branch
            statments_resolve(root->child_list[1]);
        } else {
            if (root->child_list.size() == 3) { // else branch
                statments_resolve(root->child_list[2]);
            }
        }
        return;
    }
    if (root->peek_value() == "while") {
        assert(root->child_list.size() == 2);
        while (expression_resolve(root->child_list[0])) {
            statments_resolve(root->child_list[1]);
        }
        return;
    }
    if (root->peek_value() == "return") {
        assert(root->child_list.size() <= 1);
        Scope* belong_func_scope = scopeManager.find_last_scope_with_mark("Func");
        assert(belong_func_scope != nullptr);
        string* value_addr = belong_func_scope->get_variable("ret-value");
        string* terminate_addr = belong_func_scope->get_variable("terminate-value");
        assert(value_addr != nullptr);
        assert(terminate_addr != nullptr);

        if (root->child_list.size() == 1) {
            int return_value = expression_resolve(root->child_list[0]);
            *value_addr = to_string(return_value);
        }
        *terminate_addr = "true";
        return;
    }
    if (root->peek_value() == "var") {
        assert(root->child_list.size() == 1);
        if (root->child_list[0]->peek_value() == "=") { // var name = xx;
            scopeManager.new_variable(root->child_list[0]->child_list[0]->peek_value());
            expression_resolve(root->child_list[0]);
        } else {  // var name;
            scopeManager.new_variable(root->child_list[0]->peek_value());
        }
        return;
    }
    if (root->peek_value() == "=") {
        assert(root->child_list.size() == 2);
        expression_resolve(root);
        return;
    }
    if (root->peek_type() == STATEMENTS) {
        statments_resolve(root);
        return;
    }
    if (root->peek_type() == FUNCTION) {
        if (root->peek_value() == "(") { // func call
            function_exe(root);
        } else { // func define
            function_define(root);
        }
        return;
    }
    printf("unknown statement %s\n", root->peek_value().c_str());
    exit(-1);
}

}