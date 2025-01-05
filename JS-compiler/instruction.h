#pragma once
#include <stdint.h>

// 0x07	add dst, imm
// 0x0f	add dst, src
// 0x17	sub dst, imm
// 0x1f	sub dst, src
// 0x27	mul dst, imm
// 0x2f	mul dst, src
// 0x37	div dst, imm
// 0x3f	div dst, src
// 0x97	mod dst, imm
// 0x9f	mod dst, src

// 0xb7	mov dst, imm
// 0xbf	mov dst, src
// 0x61	ldm dst, [src+off]
// 0x62	stm [dst+off], imm
// 0x63	stm [dst+off], src

// 0x05	jmp imm
// 0x15	jcz imm
// 0x55	jcnz imm
// 0x85	call imm
// 0x95	ret

// 0x03 push src
// 0x13 pop src

// 0x04 clt dst, src
// 0x14 cge dst, src
// 0x24 cle dst, src
// 0x34 ceq dst, src

// 0x09 halt

#define ADD_IMM 0x07
#define ADD_REG 0x0f
#define SUB_IMM 0x17
#define SUB_REG 0x1f
#define MUL_IMM 0x27
#define MUL_REG 0x2f
#define DIV_IMM 0x37
#define DIV_REG 0x3f
#define NEG_REG 0x87
#define MOD_IMM 0x97
#define MOD_REG 0x9f

#define MOV_IMM 0xb7
#define MOV_REG 0xbf
#define LDM_REG 0x61
#define STM_IMM 0x62
#define STM_REG 0x63

#define JMP_IMM 0x05
#define JCZ_IMM 0x15
#define JCNZ_IMM 0x55
#define CALL_IMM 0x85
#define RET     0x95

#define PUSH_REG 0x03
#define POP_REG 0x13

#define CLT_REG 0x04
#define CGE_REG 0x14
#define CLE_REG 0x24
#define CEQ_REG 0x34
#define CGT_REG 0x44

#define HALT    0x09

typedef struct __attribute__((packed)) vm_instruction {
    uint8_t opcode;
    uint8_t dst: 4;
    uint8_t src: 4;
    int16_t offset;
    int32_t imm;
    void print_vmi() {
        printf("%#16x %#16x %#16x %#16x %#16x\n", opcode, dst, src, offset, imm);
    }
} vmi_t;

#define INSTRUCTION_SIZE (sizeof(struct vm_instruction))