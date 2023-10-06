#pragma once
#include <vector>
#include <string>
#include <map>
#include <assert.h>
#include "function_split.h"
#include "lex_split.h"
#include "../instruction.h"
#define INSTRUCTION_BUF_SIZE (5 * 1024)

using namespace std;
namespace simple_compiler {

class Assembler {
    private:
    vector<vector<string>>& asm_functions;
    int instruction_counter = 0;
    map<string, int> label_map;

    public:
    unsigned char *instruction_buf = nullptr;

    public:
    Assembler(FunctionSplitter& fs): asm_functions(fs.functions) {
        instruction_buf = (unsigned char *)malloc(INSTRUCTION_BUF_SIZE);
        assert(instruction_buf != nullptr);
        memset(instruction_buf, 0, INSTRUCTION_BUF_SIZE);
    }

    ~Assembler() {
        free(instruction_buf);
    }

    private:
    void map_label_to_address();
    void replace_label_to_address();
    string register_alias(string& reg);
    int register_index(string& reg);
    void write_instruction_to_buf(struct vm_instruction& vmi);
    void assemble_mov(vector<string>& lex_tk, struct vm_instruction *vmi);
    void assemble_stack_control(vector<string>& lex_tk, struct vm_instruction *vmi);
    void assemble_stm(vector<string>& lex_tk, struct vm_instruction *vmi);
    void assemble_ldm(vector<string>& lex_tk, struct vm_instruction *vmi);
    void assemble_arithmetic(vector<string>& lex_tk, struct vm_instruction *vmi);
    void assemble_jump_control(vector<string>& lex_tk, struct vm_instruction *vmi);
    void assemble_logic_comparison(vector<string>& lex_tk, struct vm_instruction *vmi);
    void assemble_halt(vector<string>& lex_tk, struct vm_instruction *vmi);
    void assemble_line(vector<string>& lex_tk);
    void assemble_lines();

    public:
    void do_assemble();
    void print_instruction_buf();
};
}