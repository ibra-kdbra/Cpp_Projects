#include "asm_compiler.h"
#include "lex_split.h"

using namespace std;
using namespace simple_compiler;
int AsmCompiler::get_reserve_stack_size(vector<string>& function) {
    int max = -1;
    for (vector<string>::iterator it = function.begin(); it != function.end(); ++it) {
        const char *line = it->c_str();
        const char *start, *end;
        if ((end = strstr(line, ":=")) && (start = strchr(line, 't')) && (start < end)) {
            int index = (int)strtol(start + 1, (char **)&end, 10);
            max = index > max ? index : max;
        }
    }
    return max + 1;
}

void AsmCompiler::assemble_function_start(vector<string>& function) {
    sprintf(&asm_buf[strlen(asm_buf)], "%s\n", function[0].c_str()); // .func_start
    sprintf(&asm_buf[strlen(asm_buf)], "\tpush ebp\n");
    sprintf(&asm_buf[strlen(asm_buf)], "\tmov ebp, esp\n");
    sprintf(&asm_buf[strlen(asm_buf)], "\tsub esp, %lu\n", sizeof(uint32_t) * get_reserve_stack_size(function));
    map<string, string> tmp;
    function_variables_scope.push_back(tmp);
}

void AsmCompiler::assemble_function_args_delete(vector<string>& function) {
    sprintf(&asm_buf[strlen(asm_buf)], "\tadd esp, %lu\n", sizeof(uint32_t) * get_reserve_stack_size(function));
    sprintf(&asm_buf[strlen(asm_buf)], "\tpop ebp\n");
    function_variables_scope.pop_back();
}
    
void AsmCompiler::assemble_function_args_receive(vector<string>& function) {
    if (function[0] != ".start:") {  //main function 
        const char *line = function[0].c_str();
        char *start = (char *)get_last_nth_of_char(line, '_', 2);
        char *end = (char *)get_last_nth_of_char(line, '_', 1);
        assert(start != nullptr && end != nullptr);
        int argc = strtol(start + 1, &end, 10);
        for (int i = 0; i < argc; ++i) {
            sprintf(&asm_buf[strlen(asm_buf)], "\tldm r0, ebp + %lu\n", (argc + 1 - i) * sizeof(uint32_t));
            sprintf(&asm_buf[strlen(asm_buf)], "\tstm ebp - %lu, r0\n", (i + 1) * sizeof(uint32_t));
        }
    }
}

void AsmCompiler::assemble_function_call(string line) {
    vector<string> lex_tk = split_line(line);
    assert(lex_tk[0] == "call");
    assert(lex_tk.size() >= 2);
    for (int i = 0; i < lex_tk.size() - 2; ++i) {
        sprintf(&asm_buf[strlen(asm_buf)], "\tldm r0, ebp - %lu\n", 
            (get_variable_index(lex_tk[i + 2]) + 1) * sizeof(uint32_t));
        sprintf(&asm_buf[strlen(asm_buf)], "\tpush r0\n");
    }
    sprintf(&asm_buf[strlen(asm_buf)], "\tcall %s\n", lex_tk[1].c_str());
    if (lex_tk.size() > 2) {
        sprintf(&asm_buf[strlen(asm_buf)], "\tadd esp, %lu\n", 
            (lex_tk.size() - 2 )* sizeof(uint32_t));
    }
}

void AsmCompiler::set_mapping_variable(string alloc_variable, string mapping_variable) {
    int size = function_variables_scope.size();
    map<string, string>::iterator it = function_variables_scope[size - 1].find(alloc_variable);
    assert(it == function_variables_scope[size - 1].end());
    function_variables_scope[size - 1].insert(make_pair(alloc_variable, mapping_variable));
}

string AsmCompiler::get_mapping_variable(string alloc_variable) {
    int size = function_variables_scope.size();
    map<string, string>::iterator it = function_variables_scope[size - 1].find(alloc_variable);
    assert(it != function_variables_scope[size - 1].end());
    return it->second;
}

void AsmCompiler::process_line(string line) {  // may have :=
    vector<string> lex_tk = split_line(line);
    if (lex_tk.size() == 1 && lex_tk[0].c_str()[0] == '.') {
        sprintf(&asm_buf[strlen(asm_buf)], "%s\n", lex_tk[0].c_str());
        return;
    }
    if (lex_tk.size() >= 3 && lex_tk[2] == "alloc") {
        set_mapping_variable(lex_tk[3], lex_tk[0]);
        return;
    }
    if (lex_tk.size() >= 3 && lex_tk[2] == "read") {
        sprintf(&asm_buf[strlen(asm_buf)], "\tldm r0, ebp - %lu\n", 
            sizeof(uint32_t) * (get_variable_index(get_mapping_variable(lex_tk[3])) + 1));
        sprintf(&asm_buf[strlen(asm_buf)], "\tstm ebp - %lu, r0\n", 
            sizeof(uint32_t) * (get_variable_index(lex_tk[0]) + 1));
        return;
    }
    if (lex_tk.size() >= 3 && lex_tk[2] == "write") {
        sprintf(&asm_buf[strlen(asm_buf)], "\tldm r0, ebp - %lu\n", 
            sizeof(uint32_t) * (get_variable_index(lex_tk[3]) + 1));
        sprintf(&asm_buf[strlen(asm_buf)], "\tstm ebp - %lu, r0\n", 
            sizeof(uint32_t) * (get_variable_index(get_mapping_variable(lex_tk[0])) + 1));
        return;
    }
    if (lex_tk.size() == 5 && 
        (lex_tk[3] == "+" || lex_tk[3] == "-" || lex_tk[3] == "*" || lex_tk[3] == "/" ||
            lex_tk[3] == "%" || lex_tk[3] == ">" || lex_tk[3] == "<" || lex_tk[3] == ">=" ||
            lex_tk[3] == "<=" || lex_tk[3] == "==")) {
        sprintf(&asm_buf[strlen(asm_buf)], "\tldm r0, ebp - %lu\n", 
            sizeof(uint32_t) * (get_variable_index(lex_tk[2]) + 1));
        sprintf(&asm_buf[strlen(asm_buf)], "\tldm r1, ebp - %lu\n", 
            sizeof(uint32_t) * (get_variable_index(lex_tk[4]) + 1));
        if (lex_tk[3] == "+") {
            sprintf(&asm_buf[strlen(asm_buf)], "\tadd r0, r1\n");
        }
        if (lex_tk[3] == "-") {
            sprintf(&asm_buf[strlen(asm_buf)], "\tsub r0, r1\n");
        }
        if (lex_tk[3] == "*") {
            sprintf(&asm_buf[strlen(asm_buf)], "\tmul r0, r1\n");
        }
        if (lex_tk[3] == "/") {
            sprintf(&asm_buf[strlen(asm_buf)], "\tdiv r0, r1\n");
        }
        if (lex_tk[3] == "%") {
            sprintf(&asm_buf[strlen(asm_buf)], "\tmod r0, r1\n");
        }
        if (lex_tk[3] == ">") {
            sprintf(&asm_buf[strlen(asm_buf)], "\tcgt r0, r1\n");
        }
        if (lex_tk[3] == "<") {
            sprintf(&asm_buf[strlen(asm_buf)], "\tclt r0, r1\n");
        }
        if (lex_tk[3] == ">=") {
            sprintf(&asm_buf[strlen(asm_buf)], "\tcge r0, r1\n");
        }
        if (lex_tk[3] == "<=") {
            sprintf(&asm_buf[strlen(asm_buf)], "\tcle r0, r1\n");
        }
        if (lex_tk[3] == "==") {
            sprintf(&asm_buf[strlen(asm_buf)], "\tceq r0, r1\n");
        }
        sprintf(&asm_buf[strlen(asm_buf)], "\tstm ebp - %lu, r0\n", 
            sizeof(uint32_t) * (get_variable_index(lex_tk[0]) + 1));
        return;
    }
    if (lex_tk.size() >= 2 && lex_tk[0] == "call") {
        assemble_function_call(line);
        return;
    }
    if (lex_tk.size() >= 4 && lex_tk[2] == "call") {
        assemble_function_call(line.substr(line.find("call")));
        sprintf(&asm_buf[strlen(asm_buf)], "\tstm ebp - %lu, r0\n", 
            sizeof(uint32_t) * (get_variable_index(lex_tk[0]) + 1));
        return;
    }
    if (lex_tk.size() == 3 && lex_tk[1] == ":=") {
        if (lex_tk[2].c_str()[0] == 't') {
            sprintf(&asm_buf[strlen(asm_buf)], "\tldm r0, ebp - %lu\n", 
                sizeof(uint32_t) * (get_variable_index(lex_tk[2]) + 1));
            sprintf(&asm_buf[strlen(asm_buf)], "\tstm ebp - %lu, r0\n", 
                sizeof(uint32_t) * (get_variable_index(lex_tk[0]) + 1));
        } else {
            sprintf(&asm_buf[strlen(asm_buf)], "\tstm ebp - %lu, %s\n", 
                sizeof(uint32_t) * (get_variable_index(lex_tk[0]) + 1), lex_tk[2].c_str());
        }
        return;
    }
    if (lex_tk.size() == 1 && (lex_tk[0] == "return" || lex_tk[0] == "halt")) {
        if (lex_tk[0] == "return") {
            sprintf(&asm_buf[strlen(asm_buf)], "\tret\n");
        } else {
            sprintf(&asm_buf[strlen(asm_buf)], "\thalt\n");
        }
        return;
    }
    if (lex_tk.size() == 2 && lex_tk[0] == "jmp") {
        sprintf(&asm_buf[strlen(asm_buf)], "\tjmp %s\n", lex_tk[1].c_str());
        return;
    }
    if (lex_tk.size() == 2 && lex_tk[0] == "save") {
        sprintf(&asm_buf[strlen(asm_buf)], "\tldm r0, ebp - %lu\n", 
            sizeof(uint32_t) * (get_variable_index(lex_tk[1]) + 1));
        return;
    }
    if (lex_tk.size() == 3 && (lex_tk[0] == "jcz" || lex_tk[0] == "jcnz")) {
        sprintf(&asm_buf[strlen(asm_buf)], "\tldm r0, ebp - %lu\n", 
            sizeof(uint32_t) * (get_variable_index(lex_tk[1]) + 1));
        sprintf(&asm_buf[strlen(asm_buf)], "\t%s %s\n", lex_tk[0].c_str(), lex_tk[2].c_str());
        return;
    }
    if (lex_tk.size() == 4 && lex_tk[2] == "-") {
        sprintf(&asm_buf[strlen(asm_buf)], "\tldm r0, ebp - %lu\n", 
            sizeof(uint32_t) * (get_variable_index(lex_tk[3]) + 1));
        sprintf(&asm_buf[strlen(asm_buf)], "\tneg r0\n");
        sprintf(&asm_buf[strlen(asm_buf)], "\tstm ebp - %lu, r0\n", 
            sizeof(uint32_t) * (get_variable_index(lex_tk[0]) + 1));
        return;
    }
    if (lex_tk.size() == 4 && lex_tk[2] == "+") {
        return;
    }

    printf("unsupport IR: ");
    for (int i = 0; i < lex_tk.size(); ++i) {
        printf ("%s ", lex_tk[i].c_str());
    }
    printf("\n");
    exit(-1);
}

void AsmCompiler::process_lines() {
    for (vector<vector<string>>::iterator it = IR_functions.begin();
        it != IR_functions.end(); ++it) {
        assemble_function_start(*it);
        assemble_function_args_receive(*it);
        // we want to skip the start: .func_startXXX and the end: ret/halt
        for (vector<string>::iterator iit = it->begin() + 1; iit != it->end() - 1; ++iit) {
            process_line(*iit);
        }
        assemble_function_args_delete(*it);
        process_line((*it)[it->size() - 1]); // process the ret/halt
    }
}