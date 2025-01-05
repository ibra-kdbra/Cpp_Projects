#pragma once
#include "function_split.h"
#include <assert.h>
#include <map>
#define ASM_BUF_SIZE (10 * 1024)

using namespace std;
namespace simple_compiler {

class AsmCompiler {
    private:
    vector<vector<string>>& IR_functions;
    vector<map<string, string>> function_variables_scope;

    public:
    AsmCompiler(FunctionSplitter& fs): IR_functions(fs.functions) {
        asm_buf = (char *)malloc(ASM_BUF_SIZE);
        assert(asm_buf != nullptr);
        memset(asm_buf, 0, ASM_BUF_SIZE);
    }

    ~AsmCompiler() {
        free(asm_buf);
    }

    char *asm_buf = nullptr;

    int get_reserve_stack_size(vector<string>& function);
    void assemble_function_start(vector<string>& function);
    void assemble_function_args_delete(vector<string>& function);
    void assemble_function_args_receive(vector<string>& function);
    void assemble_function_call(string line);
    void set_mapping_variable(string alloc_variable, string mapping_variable);
    string get_mapping_variable(string alloc_variable);
    void process_line(string line);
    void process_lines();
};

};