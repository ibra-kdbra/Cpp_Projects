#pragma once
#include "../parser/parser.h"
#include "../parser/ast_token.h"
#include <string>
#include <cstdlib>
#include <cstring>
#define IR_BUF_SIZE (10 * 1024)

using namespace simple_parser;
using namespace std;
namespace simple_compiler {

struct function_info {
    AstToken *func_root;
    string func_signature;
};

class IRCompiler {
    private:
    Parser& parser_instance;
    vector<function_info> function_define_list;
    vector<int> variable_index_list;
    vector<string> function_scope;

    int func_replica_count = 0;

    public:
    char *IR_buf = nullptr;

    public:
    IRCompiler(Parser &p): parser_instance(p) {
        variable_index_list.push_back(0); // inital global env
        //todo: unsafe size here
        IR_buf = (char *)malloc(IR_BUF_SIZE);
        assert(IR_buf != nullptr);
        memset(IR_buf, 0, IR_BUF_SIZE);
    }

    ~IRCompiler() {
        free(IR_buf);
    }

    int expression_trans(AstToken *exp, int *variable_index);
    void statement_trans(AstToken *root);
    void statements_trans(AstToken *root);
    void if_statement_trans(AstToken *token);
    void while_statement_trans(AstToken *token);
    void get_local_var(AstToken *statements_root, vector<string>& local_var);
    void allocate_local_var(vector<string>& local_var);
    void delete_local_var(vector<string>& local_var);
    void function_def_statement_tran(AstToken *token);
    void print_function();
    string find_calling_function(string name);
    void function_call_statement_tran(AstToken *token);
    void compile_to_IR();
};

}