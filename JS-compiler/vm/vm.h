#pragma once
#include <cstdlib>
#include "../compiler/assembler.h"
#include "../instruction.h"
#define STACK_SIZE (10 * 1024)

#define ADD_HANDLER(INS) \
    inst_handler_table[INS] = &INS##_handler;

using namespace std;
namespace simple_vm {

class VM {

    private:
    unsigned char *instruction_buf = nullptr;
    struct cpu {
        union {
            uint32_t reg[11];
            struct {
                uint32_t r0;
                uint32_t r1;
                uint32_t notused[6];
                uint32_t esp; // r8
                uint32_t ebp; // r9
                uint32_t eip; // r10
            };
        };
    } cpu;
    void (*inst_handler_table[0xff])(VM&, vmi_t&);

    uint8_t *stack_start = nullptr;
    uint8_t *stack_end = nullptr;

    public:
    VM(simple_compiler::Assembler& assembler): instruction_buf(assembler.instruction_buf) {
        stack_start = (uint8_t *)malloc(STACK_SIZE);
        assert(stack_start != nullptr);
        memset(stack_start, 0, STACK_SIZE);
        memset(&cpu, 0, sizeof(cpu));
        memset(inst_handler_table, 0, sizeof(inst_handler_table));
        stack_end = stack_start + STACK_SIZE;
        cpu.ebp = STACK_SIZE;
        cpu.esp = STACK_SIZE;
        cpu.eip = 0;

        init_handler();
    }

    ~VM() {
        free(stack_start);
    }

    uint64_t fetch_instruction() {
        uint64_t ret =  *(uint64_t *)(instruction_buf + cpu.eip);
        cpu.eip += INSTRUCTION_SIZE;
        return ret;
    }

    // 0x07
    static void ADD_IMM_handler(VM& vm, vmi_t& inst) {
        vm.cpu.reg[inst.dst] += inst.imm;
    }
    // 0x0f
    static void ADD_REG_handler(VM& vm, vmi_t& inst) {
        vm.cpu.reg[inst.dst] += vm.cpu.reg[inst.src];
    }
    // 0x17
    static void SUB_IMM_handler(VM& vm, vmi_t& inst) {
        vm.cpu.reg[inst.dst] -= inst.imm;
    }
    // 0x1f
    static void SUB_REG_handler(VM& vm, vmi_t& inst) {
        vm.cpu.reg[inst.dst] -= vm.cpu.reg[inst.src];
    }
    // 0x27
    static void MUL_IMM_handler(VM& vm, vmi_t& inst) {
        vm.cpu.reg[inst.dst] *= inst.imm;
    }
    // 0x2f
    static void MUL_REG_handler(VM& vm, vmi_t& inst) {
        vm.cpu.reg[inst.dst] *= vm.cpu.reg[inst.src];
    }
    // 0x37
    static void DIV_IMM_handler(VM& vm, vmi_t& inst) {
        vm.cpu.reg[inst.dst] /= inst.imm;
    }
    // 0x3f
    static void DIV_REG_handler(VM& vm, vmi_t& inst) {
        vm.cpu.reg[inst.dst] /= vm.cpu.reg[inst.src];
    }
    // 0x87
    static void NEG_REG_handler(VM& vm, vmi_t& inst) {
        vm.cpu.reg[inst.dst] = 0 - vm.cpu.reg[inst.dst];
    }
    // 0x97
    static void MOD_IMM_handler(VM& vm, vmi_t& inst) {
        vm.cpu.reg[inst.dst] %= inst.imm;
    }
    // 0x9f
    static void MOD_REG_handler(VM& vm, vmi_t& inst) {
        vm.cpu.reg[inst.dst] %= vm.cpu.reg[inst.src];
    }
    // 0xb7
    static void MOV_IMM_handler(VM& vm, vmi_t& inst) {
        vm.cpu.reg[inst.dst] = inst.imm;
    }
    // 0xbf
    static void MOV_REG_handler(VM& vm, vmi_t& inst) {
        vm.cpu.reg[inst.dst] = vm.cpu.reg[inst.src];
    }
    // 0x61
    static void LDM_REG_handler(VM& vm, vmi_t& inst) {
        vm.cpu.reg[inst.dst] = *(uint32_t *)(vm.cpu.reg[inst.src] + inst.offset + vm.stack_start);
    }
    // 0x62
    static void STM_IMM_handler(VM& vm, vmi_t& inst) {
        *(uint32_t *)(vm.cpu.reg[inst.dst] + inst.offset + vm.stack_start) = inst.imm;
    }
    // 0x63
    static void STM_REG_handler(VM& vm, vmi_t& inst) {
        *(uint32_t *)(vm.cpu.reg[inst.dst] + inst.offset + vm.stack_start) = vm.cpu.reg[inst.src];
    }
    // 0x05
    static void JMP_IMM_handler(VM& vm, vmi_t& inst) {
        vm.cpu.eip = inst.imm;
    }
    // 0x15
    static void JCZ_IMM_handler(VM& vm, vmi_t& inst) {
        if (vm.cpu.r0 == 0) {
            vm.cpu.eip = inst.imm;
        }
    }
    // 0x55
    static void JCNZ_IMM_handler(VM& vm, vmi_t& inst) {
        if (vm.cpu.r0 != 0) {
            vm.cpu.eip = inst.imm;
        }
    }
    // 0x85
    static void CALL_IMM_handler(VM& vm, vmi_t& inst) {
        vm.cpu.esp -= sizeof(uint32_t);
        *(uint32_t *)(vm.stack_start + vm.cpu.esp) = vm.cpu.eip;
        vm.cpu.eip = inst.imm;
        // print_statck(vm);
    }
    // 0x85
    static void RET_handler(VM& vm, vmi_t& inst) {
        vm.cpu.eip = *(uint32_t *)(vm.stack_start + vm.cpu.esp);
        vm.cpu.esp += sizeof(uint32_t);
        printf("return %#16x\n", vm.cpu.r0);
        // print_statck(vm);
    }

    // 0x03
    static void PUSH_REG_handler(VM& vm, vmi_t& inst) {
        vm.cpu.esp -= sizeof(uint32_t);
        *(uint32_t *)(vm.stack_start + vm.cpu.esp) = vm.cpu.reg[inst.src];
        // print_statck(vm);
    }
    // 0x13
    static void POP_REG_handler(VM& vm, vmi_t& inst) {
        vm.cpu.reg[inst.dst] = *(uint32_t *)(vm.stack_start + vm.cpu.esp);
        vm.cpu.esp += sizeof(uint32_t);
        // print_statck(vm);
    }
    // 0x04
    static void CLT_REG_handler(VM& vm, vmi_t& inst) {
        if (vm.cpu.reg[inst.dst] < vm.cpu.reg[inst.src]) {
            vm.cpu.r0 = 1;
        } else {
            vm.cpu.r0 = 0;
        }
    }
    // 0x14
    static void CGE_REG_handler(VM& vm, vmi_t& inst) {
        if (vm.cpu.reg[inst.dst] >= vm.cpu.reg[inst.src]) {
            vm.cpu.r0 = 1;
        } else {
            vm.cpu.r0 = 0;
        }
    }
    // 0x24
    static void CLE_REG_handler(VM& vm, vmi_t& inst) {
        if (vm.cpu.reg[inst.dst] <= vm.cpu.reg[inst.src]) {
            vm.cpu.r0 = 1;
        } else {
            vm.cpu.r0 = 0;
        }
    }
    // 0x34
    static void CEQ_REG_handler(VM& vm, vmi_t& inst) {
        if (vm.cpu.reg[inst.dst] == vm.cpu.reg[inst.src]) {
            vm.cpu.r0 = 1;
        } else {
            vm.cpu.r0 = 0;
        }
    }
    // 0x44
    static void CGT_REG_handler(VM& vm, vmi_t& inst) {
        if (vm.cpu.reg[inst.dst] > vm.cpu.reg[inst.src]) {
            vm.cpu.r0 = 1;
        } else {
            vm.cpu.r0 = 0;
        }
    }
    // 0x09
    static void HALT_handler(VM& vm, vmi_t& inst) {
        printf("program exit normally");
        exit(0);
    }

    void decode_instruction(uint64_t inst) {
        vmi_t& inst_p = *(vmi_t *)&inst;
        inst_p.print_vmi();
        inst_handler_table[inst & 0xff](*this, inst_p);
    }

    void run() {
        while (cpu.eip < INSTRUCTION_BUF_SIZE) {
            assert(cpu.ebp >= cpu.esp);
            uint64_t instruction = fetch_instruction();
            decode_instruction(instruction);
        }
    }

    static void print_statck(VM& vm) {
        printf("+++++++++\n");
        for (int i = 0; vm.cpu.esp + i <=  STACK_SIZE; i+=4) {
            printf("%#12x", *(uint32_t *)(vm.cpu.esp + i + vm.stack_start));
        }
        printf("\n-------\n");
    }

    void init_handler() {
        ADD_HANDLER(ADD_IMM);
        ADD_HANDLER(ADD_REG);
        ADD_HANDLER(SUB_IMM);
        ADD_HANDLER(SUB_REG);
        ADD_HANDLER(MUL_IMM);
        ADD_HANDLER(MUL_REG);
        ADD_HANDLER(DIV_IMM);
        ADD_HANDLER(DIV_REG);
        ADD_HANDLER(NEG_REG);
        ADD_HANDLER(MOD_IMM);
        ADD_HANDLER(MOD_REG);
        ADD_HANDLER(MOV_IMM);
        ADD_HANDLER(MOV_REG);
        ADD_HANDLER(LDM_REG);
        ADD_HANDLER(STM_IMM);
        ADD_HANDLER(STM_REG);
        ADD_HANDLER(JMP_IMM);
        ADD_HANDLER(JCZ_IMM);
        ADD_HANDLER(JCNZ_IMM);
        ADD_HANDLER(CALL_IMM);
        ADD_HANDLER(RET);
        ADD_HANDLER(PUSH_REG);
        ADD_HANDLER(POP_REG);
        ADD_HANDLER(CLT_REG);
        ADD_HANDLER(CGE_REG);
        ADD_HANDLER(CLE_REG);
        ADD_HANDLER(CEQ_REG);
        ADD_HANDLER(CGT_REG);
        ADD_HANDLER(HALT);
    }

};

}