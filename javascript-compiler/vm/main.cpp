#include "../lexser/lexser.h"
#include "../parser/parser.h"
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include "../compiler/function_split.h"
#include "../compiler/asm_compiler.h"
#include "../compiler/assembler.h"
#include "../compiler/ir_compiler.h"
#include "vm.h"
#define F_SIZE (10 * 1024)

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("usage: ./a.out file\n");
        return -1;
    }

    char *buf = (char *)malloc(sizeof(char) * F_SIZE);
    assert(buf != NULL);

    int fd = open(argv[1], O_RDONLY);
    assert(fd > 0);
    read(fd, buf, F_SIZE);

    simple_lexser::Lexser lexser;
    lexser.lex(buf);
    
    simple_parser::Parser parser(lexser);
    parser.parse();
    printf("\n");

    simple_compiler::IRCompiler ir_compiler(parser);
    ir_compiler.compile_to_IR();
    printf("-----------------\n");

    simple_compiler::FunctionSplitter irFunctionSpliter(ir_compiler.IR_buf);
    irFunctionSpliter.print_functions();

    printf ("----------------\n");
    simple_compiler::AsmCompiler asm_compiler(irFunctionSpliter);
    asm_compiler.process_lines();

    simple_compiler::FunctionSplitter asmFunctionSpliter(asm_compiler.asm_buf);
    asmFunctionSpliter.print_functions();

    printf ("----------------\n");

    simple_compiler::Assembler assembler(asmFunctionSpliter);
    assembler.do_assemble();
    assembler.print_instruction_buf();

    printf ("----------------\n");
    simple_vm::VM vm(assembler);
    vm.run();
    return 0;
}