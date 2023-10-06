#include <vector>
#include <string>
#include <map>
#include <assert.h>
#include "assembler.h"

using namespace std;
using namespace simple_compiler;

void Assembler::map_label_to_address() {
    for (vector<vector<string>>::iterator it = asm_functions.begin(); it != asm_functions.end(); ++it) {
        for (vector<string>::iterator iit = it->begin(); iit != it->end(); ++iit) {
            if (iit->c_str()[0] == '.') {  // it's a label
                assert(iit->c_str()[strlen(iit->c_str()) - 1] == ':');
                string label = iit->substr(1, iit->find(":") - 1);
                map<string, int>::iterator itr = label_map.find(label);
                assert(itr == label_map.end());
                label_map.insert(make_pair(label, instruction_counter));
            } else {
                ++instruction_counter;
            }
        }
    }
}

void Assembler::replace_label_to_address() {
    for (vector<vector<string>>::iterator it = asm_functions.begin(); it != asm_functions.end(); ++it) {
        for (vector<string>::iterator iit = it->begin(); iit != it->end(); ++iit) {
            vector<string> lex_tk = split_line(*iit);
            if ((lex_tk[0] == "jmp") || (lex_tk[0] == "jcz") 
                || (lex_tk[0] == "jcnz") || (lex_tk[0] == "call")) {
                assert(lex_tk.size() == 2);
                string label;
                if (lex_tk[0] == "call") 
                    label = lex_tk[1];
                else
                    label = lex_tk[1].substr(1);
                map<string, int>::iterator itr = label_map.find(label);
                assert(itr != label_map.end());
                int address = itr->second * INSTRUCTION_SIZE;
                *iit = lex_tk[0] + " " + to_string(address);
            }
        }
    }
}

string Assembler::register_alias(string& reg) {
    if (reg == "esp")
        return "r8";
    if (reg == "ebp")
        return "r9";
    return reg;
}

int Assembler::register_index(string& reg) {
    const char *str = register_alias(reg).c_str();
    assert(*str == 'r');
    int ret = atoi(str + 1);
    assert(ret <= 0xf);
    return ret;
}

void Assembler::write_instruction_to_buf(struct vm_instruction& vmi) {
    static int instruction_counter = 0;
    memcpy(instruction_buf + instruction_counter * INSTRUCTION_SIZE, 
        &vmi, sizeof(struct vm_instruction));
    ++instruction_counter;
    assert(instruction_counter < INSTRUCTION_BUF_SIZE / INSTRUCTION_SIZE);
}

// 0xb7	mov dst, imm
// 0xbf	mov dst, src
void Assembler::assemble_mov(vector<string>& lex_tk, struct vm_instruction *vmi) {
    assert(lex_tk.size() == 3);
    if (register_alias(lex_tk[2]).c_str()[0] == 'r') {  // mov dst, src
        vmi->opcode = 0xbf;
        vmi->src = register_index(lex_tk[2]);
    } else {  // mov dst, imm
        vmi->opcode = 0xb7;
        vmi->imm = atoi(lex_tk[2].c_str());
    }
    vmi->dst = register_index(lex_tk[1]);
}

// 0x03 push src
// 0x13 pop dst
void Assembler::assemble_stack_control(vector<string>& lex_tk, struct vm_instruction *vmi) {
    assert(lex_tk.size() == 2);
    if (lex_tk[0] == "push") {
        vmi->opcode = 0x03;
        vmi->src = register_index(lex_tk[1]);
    }
    if (lex_tk[0] == "pop") {
        vmi->opcode = 0x13;
        vmi->dst = register_index(lex_tk[1]);
    }
}

// 0x62	stm [dst+off], imm
// 0x63	stm [dst+off], src
void Assembler::assemble_stm(vector<string>& lex_tk, struct vm_instruction *vmi) {
    assert(lex_tk.size() >= 3);
    if (register_alias(lex_tk[lex_tk.size() - 1]).c_str()[0] == 'r') {  // stm xx, src
        vmi->src = register_index(lex_tk[lex_tk.size() -1]);
        vmi->opcode = 0x63;
    } else {  // stm xx, imm
        vmi->imm = atoi(lex_tk[lex_tk.size() - 1].c_str());
        vmi->opcode = 0x62;
    }
    vmi->dst = register_index(lex_tk[1]);
    if (lex_tk.size() > 3) {  // offset exist
        assert(lex_tk.size() == 5);
        vmi->offset = atoi((lex_tk[2] + lex_tk[3]).c_str());
    }
}

// 0x61	ldm dst, [src+off]
void Assembler::assemble_ldm(vector<string>& lex_tk, struct vm_instruction *vmi) {
    assert(lex_tk.size() >= 3);
    vmi->opcode = 0x61;
    vmi->dst = register_index(lex_tk[1]);
    vmi->src = register_index(lex_tk[2]);
    if (lex_tk.size() > 3) {  // offset exist
        assert(lex_tk.size() == 5);
        vmi->offset = atoi((lex_tk[3] + lex_tk[4]).c_str());
    }
}

// 0x07	add dst, imm
// 0x0f	add dst, src
// 0x17	sub dst, imm
// 0x1f	sub dst, src
// 0x27	mul dst, imm
// 0x2f	mul dst, src
// 0x37	div dst, imm
// 0x3f	div dst, src
// 0x87 neg dst
// 0x97	mod dst, imm
// 0x9f	mod dst, src
void Assembler::assemble_arithmetic(vector<string>& lex_tk, struct vm_instruction *vmi) {
    if (lex_tk[0] == "neg") {
        assert(lex_tk.size() == 2);
        vmi->dst = register_index(lex_tk[1]);
        vmi->opcode = 0x87;
        return;
    }
    assert(lex_tk.size() == 3);
    bool is_imm = false;
    if (register_alias(lex_tk[2]).c_str()[0] == 'r') {  // src
        vmi->src = register_index(lex_tk[2]);
    } else {  // imm
        vmi->imm = atoi(lex_tk[2].c_str());
        is_imm = true;
    }
    vmi->dst = register_index(lex_tk[1]);
    if (lex_tk[0] == "add") {
        vmi->opcode = is_imm ? 0x07 : 0x0f;
        return;
    }
    if (lex_tk[0] == "sub") {
        vmi->opcode = is_imm ? 0x17 : 0x1f;
        return;
    }
    if (lex_tk[0] == "mul") {
        vmi->opcode = is_imm ? 0x27 : 0x2f;
        return;
    }
    if (lex_tk[0] == "div") {
        vmi->opcode = is_imm ? 0x37 : 0x3f;
        return;
    }
    if (lex_tk[0] == "mod") {
        vmi->opcode = is_imm ? 0x97 : 0x9f;
        return;
    }
}

// 0x05	jmp imm
// 0x15	jcz imm
// 0x55	jcnz imm
// 0x85	call imm
// 0x95	ret
void Assembler::assemble_jump_control(vector<string>& lex_tk, struct vm_instruction *vmi) {
    assert(lex_tk.size() <= 2);
    if (lex_tk[0] == "ret") {
        vmi->opcode = 0x95;
        return;
    }
    vmi->imm = atoi(lex_tk[1].c_str());
    if (lex_tk[0] == "jmp") {
        vmi->opcode = 0x05;
        return;
    }
    if (lex_tk[0] == "jcz") {
        vmi->opcode = 0x15;
        return;
    }
    if (lex_tk[0] == "jcnz") {
        vmi->opcode = 0x55;
        return;
    }
    if (lex_tk[0] == "call") {
        vmi->opcode = 0x85;
        return;
    }
}

// 0x04 clt dst, src
// 0x14 cge dst, src
// 0x24 cle dst, src
// 0x34 ceq dst, src
// 0x44 cgt dst, src
void Assembler::assemble_logic_comparison(vector<string>& lex_tk, struct vm_instruction *vmi) {
    assert(lex_tk.size() == 3);
    vmi->src = register_index(lex_tk[2]);
    vmi->dst = register_index(lex_tk[1]);
    if (lex_tk[0] == "clt") {
        vmi->opcode = 0x04;
        return;
    }
    if (lex_tk[0] == "cge") {
        vmi->opcode = 0x14;
        return;
    }
    if (lex_tk[0] == "cle") {
        vmi->opcode = 0x24;
        return;
    }
    if (lex_tk[0] == "ceq") {
        vmi->opcode = 0x34;
        return;
    }
    if (lex_tk[0] == "cgt") {
        vmi->opcode = 0x44;
        return;
    }
}

void Assembler::assemble_halt(vector<string>& lex_tk, struct vm_instruction *vmi) {
    assert(lex_tk.size() == 1);
    vmi->opcode = 0x09;
    return;
}

void Assembler::assemble_line(vector<string>& lex_tk) {
    struct vm_instruction vmi;
    memset(&vmi, 0, sizeof(struct vm_instruction));
    if (lex_tk[0] == "mov") {
        assemble_mov(lex_tk, &vmi);
        goto out;
    }
    if (lex_tk[0] == "push" || lex_tk[0] == "pop") {
        assemble_stack_control(lex_tk, &vmi);
        goto out;
    }
    if (lex_tk[0] == "ldm") {
        assemble_ldm(lex_tk, &vmi);
        goto out;
    }
    if (lex_tk[0] == "stm") {
        assemble_stm(lex_tk, &vmi);
        goto out;
    }
    if (lex_tk[0] == "stm") {
        assemble_stm(lex_tk, &vmi);
        goto out;
    }
    if (lex_tk[0] == "add" || lex_tk[0] == "sub" || lex_tk[0] == "neg" ||
        lex_tk[0] == "mul" || lex_tk[0] == "div" || lex_tk[0] == "mod") {
        assemble_arithmetic(lex_tk, &vmi);
        goto out;
    }
    if (lex_tk[0] == "jmp" || lex_tk[0] == "jcz" || 
        lex_tk[0] == "jcnz" || lex_tk[0] == "call" || lex_tk[0] == "ret") {
        assemble_jump_control(lex_tk, &vmi);
        goto out;
    }
    if (lex_tk[0] == "clt" || lex_tk[0] == "cge" || 
        lex_tk[0] == "cle" || lex_tk[0] == "ceq" || lex_tk[0] == "cgt") {
        assemble_logic_comparison(lex_tk, &vmi);
        goto out;
    }
    if (lex_tk[0] == "halt") {
        assemble_halt(lex_tk, &vmi);
        goto out;
    }
    printf("unknown instruction ");
    for (int i = 0; i < lex_tk.size(); ++i) {
        printf("%s ", lex_tk[i].c_str());
    }
    printf("\n");
    exit(-1);
out:
    write_instruction_to_buf(vmi);
    return;
}

void Assembler::assemble_lines() {
    for (vector<vector<string>>::iterator it = asm_functions.begin(); it != asm_functions.end(); ++it) {
        for (vector<string>::iterator iit = it->begin(); iit != it->end(); ++iit) {
            vector<string> lex_tk = split_line(*iit);
            if (lex_tk.size() == 1 && lex_tk[0].c_str()[0] == '.')
                continue;
            assemble_line(lex_tk);
        }
    }
}

void Assembler::do_assemble() {
    map_label_to_address();
    replace_label_to_address();
    assemble_lines();
}

void Assembler::print_instruction_buf() {
    for (int i = 0; i < INSTRUCTION_BUF_SIZE / INSTRUCTION_SIZE; ++i) {
        if (*(uint64_t *)(instruction_buf + i * INSTRUCTION_SIZE) == 0)
            break;
        printf("%#16lx\n", *(uint64_t *)(instruction_buf + i * INSTRUCTION_SIZE));
    }
}