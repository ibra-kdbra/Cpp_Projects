#include "ir_compiler.h"

using namespace simple_parser;
using namespace std;
namespace simple_compiler {

int IRCompiler::expression_trans(AstToken *exp, int *variable_index) {
    if (exp->peek_type() == NUM) {
        sprintf(&IR_buf[strlen(IR_buf)], "\tt%d := %s\n", *variable_index, exp->peek_value().c_str());
        ++(*variable_index);
        return 1;
    }
    if (exp->peek_type() == IDENTIFIER) {
        sprintf(&IR_buf[strlen(IR_buf)], "\tt%d := read %s\n", *variable_index, exp->peek_value().c_str());
        ++(*variable_index);
        return 1;
    }
    if (exp->peek_value() == "+" || exp->peek_value() == "-" || 
        exp->peek_value() == "*" || exp->peek_value() == "/" || exp->peek_value() == "%" ||
        exp->peek_value() == ">" || exp->peek_value() == "<" || exp->peek_value() == ">=" ||
        exp->peek_value() == "<=" || exp->peek_value() == "==") {
        assert(exp->child_list.size() <= 2);
        int num = 0;
        for (int i = 0; i < exp->child_list.size(); ++i) {
            num += expression_trans(exp->child_list[i], variable_index);
        }
        if (num == 1) {
            sprintf(&IR_buf[strlen(IR_buf)], "\tt%d := %s t%d\n", *variable_index - num, exp->peek_value().c_str(), *variable_index - num);
        } else { // num = 2
            sprintf(&IR_buf[strlen(IR_buf)], "\tt%d := t%d %s t%d\n", *variable_index - num, *variable_index - num, exp->peek_value().c_str(), *variable_index -num + 1);
        }
        *variable_index -= num - 1;
        return 1;
    }
    if (exp->peek_value() == "=") {
        expression_trans(exp->child_list[1], variable_index);
        sprintf(&IR_buf[strlen(IR_buf)], "\t%s := write t%d\n", exp->child_list[0]->peek_value().c_str(), *variable_index -= 1);
        return 1;
    }
    if (exp->peek_value() == "||") {
        static vector<int> or_label_list;
        static int count = 0;
        ++count;

        expression_trans(exp->child_list[0], variable_index);

        or_label_list.push_back(count);
        sprintf(&IR_buf[strlen(IR_buf)], "\tjcnz t%d .or_end_%d\n", *variable_index -= 1, or_label_list[or_label_list.size() - 1]);            
        expression_trans(exp->child_list[1], variable_index);
        sprintf(&IR_buf[strlen(IR_buf)], ".or_end_%d:\n", or_label_list[or_label_list.size() - 1]);
        or_label_list.pop_back();
        return 1;
    }
    if (exp->peek_value() == "&&") {
        static vector<int> and_label_list;
        static int count = 0;
        ++count;

        expression_trans(exp->child_list[0], variable_index);

        and_label_list.push_back(count);
        sprintf(&IR_buf[strlen(IR_buf)], "\tjcz t%d .and_end_%d\n", *variable_index -= 1, and_label_list[and_label_list.size() - 1]);            
        expression_trans(exp->child_list[1], variable_index);
        sprintf(&IR_buf[strlen(IR_buf)], ".and_end_%d:\n", and_label_list[and_label_list.size() - 1]);
        and_label_list.pop_back();
        return 1;
    }
    if (exp->peek_type() == FUNCTION && exp->peek_value() == "(") {
        string f_name = exp->child_list[0]->peek_value() + "_" + to_string(exp->child_list.size() - 1);
        string actual_name = find_calling_function(f_name);
        int para_num = exp->child_list.size() - 1;
        assert(actual_name != "");
        for (int i = 0; i < exp->child_list.size() - 1; ++i) {
            expression_trans(exp->child_list[i + 1], variable_index);
        }
        sprintf(&IR_buf[strlen(IR_buf)], "\tt%d := call %s ", *variable_index -= para_num, ("func_start_" + actual_name).c_str());
        for (int i = 0; i < exp->child_list.size() - 1; ++i) {
            sprintf(&IR_buf[strlen(IR_buf)], "t%d ", *variable_index + i);
        }
        sprintf(&IR_buf[strlen(IR_buf)], "\n");
        *variable_index += 1;
        return 1;
    }

    printf("not expression token < %s >, line %d index %d\n", 
        exp->peek_value().c_str(), exp->peek_line(), exp->peek_index());
    exit(-1);
}

void IRCompiler::statement_trans(AstToken *root) {
    if (root->peek_value() == "if") {
        if_statement_trans(root);
        return;
    }
    if (root->peek_value() == "while") {
        while_statement_trans(root);
        return;
    }
    if (root->peek_value() == "var") {
        expression_trans(root->child_list[0], &variable_index_list[variable_index_list.size() - 1]);
        return;
    }
    if (root->peek_value() == "=") {
        expression_trans(root, &variable_index_list[variable_index_list.size() - 1]);
        return;
    }        
    if (root->peek_value() == "return") {
        // assert(function_define_list.size() > 0);
        assert(root->child_list.size() <= 1);
        // string f_name = function_define_list[function_define_list.size() - 1].func_signature;
        if (root->child_list.size() == 0) {
            sprintf(&IR_buf[strlen(IR_buf)], "\tjmp %s\n", function_scope[function_scope.size() - 1].c_str());
        } else {
            expression_trans(root->child_list[0], &variable_index_list[variable_index_list.size() - 1]);
            sprintf(&IR_buf[strlen(IR_buf)], "\tsave t%d\n", variable_index_list[variable_index_list.size() - 1] -= 1);
            sprintf(&IR_buf[strlen(IR_buf)], "\tjmp .func_end_%s\n", function_scope[function_scope.size() - 1].c_str());
        }
        return;
    }
    if (root->peek_type() == FUNCTION) {
        if (root->peek_value() != "(") {
            struct function_info info = {
                .func_root = root,
                .func_signature = root->peek_value() + "_" +
                    to_string(root->child_list.size() - 1) + "_" +
                    to_string(func_replica_count + 1)
            };
            function_define_list.push_back(info);
            return;
        } else {
            function_call_statement_tran(root);
            return;
        }
    }
    if (root->peek_type() == STATEMENTS) {
        statements_trans(root);
        return;
    }
    printf("unknown statement %s\n", root->peek_value().c_str());
    exit(-1);
}

void IRCompiler::statements_trans(AstToken *root) {
    assert(root->peek_type() == STATEMENTS);
    for (int i = 0; i < root->child_list.size(); ++i) {
            statement_trans(root->child_list[i]);
    }
}

void IRCompiler::if_statement_trans(AstToken *token) {
    static int self_replica_count = 0;
    ++self_replica_count;
    int label = self_replica_count;

    assert(token->peek_value() == "if" && token->child_list.size() <= 3);
    sprintf(&IR_buf[strlen(IR_buf)], ".if_start_%d:\n", label);
    expression_trans(token->child_list[0], &variable_index_list[variable_index_list.size() -1]);
    sprintf(&IR_buf[strlen(IR_buf)], "\tjcnz t%d .if_true_%d\n", variable_index_list[variable_index_list.size() -1] -= 1, label);
    sprintf(&IR_buf[strlen(IR_buf)], "\tjmp .if_false_%d\n", label);

    sprintf(&IR_buf[strlen(IR_buf)], ".if_true_%d:\n", label);
    statements_trans(token->child_list[1]);
    sprintf(&IR_buf[strlen(IR_buf)], "\tjmp .if_end_%d\n", label);

    sprintf(&IR_buf[strlen(IR_buf)], ".if_false_%d:\n",label);
    if (token->child_list.size() == 3) {
        statements_trans(token->child_list[2]);
    }
    sprintf(&IR_buf[strlen(IR_buf)], ".if_end_%d:\n", label);
}

void IRCompiler::while_statement_trans(AstToken *token) {
    static int self_replica_count = 0;
    ++self_replica_count;
    int label = self_replica_count;

    assert(token->peek_value() == "while" && token->child_list.size() == 2);
    sprintf(&IR_buf[strlen(IR_buf)], ".while_%d:\n", label);
    expression_trans(token->child_list[0], &variable_index_list[variable_index_list.size() - 1]);
    sprintf(&IR_buf[strlen(IR_buf)], "\tjcnz t%d .while_true_%d\n", variable_index_list[variable_index_list.size() - 1] -= 1, label);
    sprintf(&IR_buf[strlen(IR_buf)], "\tjmp .while_end_%d\n", label);

    sprintf(&IR_buf[strlen(IR_buf)], ".while_true_%d:\n", label);
    statements_trans(token->child_list[1]);
    sprintf(&IR_buf[strlen(IR_buf)], "\tjmp .while_%d\n", label);
    sprintf(&IR_buf[strlen(IR_buf)], ".while_end_%d:\n", label);
}

void IRCompiler::get_local_var(AstToken *statements_root, vector<string>& local_var) {
    // assert(statements_root->peek_type() == STATEMENTS);
    for (int i = 0; i < statements_root->child_list.size(); ++i) {
        if (statements_root->child_list[i]->peek_value() == "var") {
            local_var.push_back(statements_root->child_list[i]->
                child_list[0]->child_list[0]->peek_value());
        }
        // we don't want to go into function define, because it's another scope
        if (statements_root->child_list[i]->peek_type() != FUNCTION) {
            get_local_var(statements_root->child_list[i], local_var);
        }
    }
}

void IRCompiler::allocate_local_var(vector<string>& local_var) {
    for (int i = 0; i < local_var.size(); ++i) {
        sprintf(&IR_buf[strlen(IR_buf)], "\tt%d := alloc %s\n", i, local_var[i].c_str());
    }
}

void IRCompiler::delete_local_var(vector<string>& local_var) {
    for (int i = local_var.size() - 1; i >= 0; --i) {
        sprintf(&IR_buf[strlen(IR_buf)], "\tdelete %s\n", local_var[i].c_str());            
    }
}

void IRCompiler::function_def_statement_tran(AstToken *token) {
    /***********preparation*********/
    ++func_replica_count;
    int label = func_replica_count;  // function index label
    vector<string> local_var;        // local var and temporary var list
    for (int i = 0; i < token->child_list.size() - 1; ++i)
        local_var.push_back(token->child_list[i]->peek_value());
    get_local_var(token->child_list[token->child_list.size() - 1], local_var);
    // each time new veriable labeling start from 0, for generate new varible scope
    variable_index_list.push_back(0);  // temporary var index
    assert(token->peek_value() != "(" && token->peek_type() == FUNCTION);
    /*********end preparation********/

    string f_name = token->peek_value() + "_" + 
        to_string(token->child_list.size() - 1) + "_" + to_string(label);
    function_scope.push_back(f_name);
    // ok, the real function translation
    sprintf(&IR_buf[strlen(IR_buf)], "\n.func_start_%s:\n", f_name.c_str());
    allocate_local_var(local_var);

    variable_index_list[variable_index_list.size() - 1] += local_var.size();
    statements_trans(token->child_list[token->child_list.size() - 1]);
    function_scope.pop_back();

    sprintf(&IR_buf[strlen(IR_buf)], ".func_end_%s:\n", f_name.c_str());
    // delete_local_var(local_var);
    sprintf(&IR_buf[strlen(IR_buf)], "\treturn\n\n");
    variable_index_list.pop_back();
}

void IRCompiler::print_function() {
    while(function_define_list.size() > 0) {
        int index = function_define_list.size() - 1;
        AstToken *tmp = function_define_list[index].func_root;
        function_def_statement_tran(tmp);
        function_define_list.erase(function_define_list.begin() + index); 
    };
}

// to get the function from the latest function scope:
/*
*  function test() {
*       function test() {
*           xxxxx;
*       }
*       test();  // we are calling the current scope function
*  }
*/
string IRCompiler::find_calling_function(string name) {
    for (vector<function_info>::reverse_iterator it = function_define_list.rbegin();
        it != function_define_list.rend(); ++it) {
            if (name == it->func_signature.substr(0, it->func_signature.find_last_of("_"))) {
                return it->func_signature;
            }
        }
    return "";
}

void IRCompiler::function_call_statement_tran(AstToken *token) {
    assert(token->peek_value() == "(" && token->peek_type() == FUNCTION);
    string name = token->child_list[0]->peek_value() 
        + "_" + to_string(token->child_list.size() - 1);
    string actual_func_name = find_calling_function(name);
    int para_num = token->child_list.size() - 1;
    assert(actual_func_name != "");

    for (int i = 0; i < para_num; ++i) {
        expression_trans(token->child_list[i + 1], 
            &variable_index_list[variable_index_list.size() - 1]);
    }
    sprintf(&IR_buf[strlen(IR_buf)], "\tcall %s ", ("func_start_" + actual_func_name).c_str());
    variable_index_list[variable_index_list.size() - 1] -= para_num;
    for (int i = 0; i < para_num; ++i) {
        sprintf(&IR_buf[strlen(IR_buf)], "t%d ", variable_index_list[variable_index_list.size() - 1] + i);
    }
    sprintf(&IR_buf[strlen(IR_buf)], "\n");
}

void IRCompiler::compile_to_IR() {
    std::vector<string> local_var;
    sprintf(&IR_buf[strlen(IR_buf)], ".start:\n");
    get_local_var(parser_instance.ast_tree_root, local_var);
    allocate_local_var(local_var);
    variable_index_list[variable_index_list.size() - 1] += local_var.size();
    statements_trans(parser_instance.ast_tree_root);  
    sprintf(&IR_buf[strlen(IR_buf)], "\thalt\n");
    print_function();
}

};