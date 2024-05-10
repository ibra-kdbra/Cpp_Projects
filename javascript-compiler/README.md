# Overview #

## Introduction ##

This is a project which I developed to learn the mechanism of compilers. The project contains 5 parts:

1) a lexer;

2) a parser;

3) an interpreter;

4) a compiler;

5) a virtual machine;

The designed programming language is similar to Javascript, but much simpler. Code samples can be found in each part's testcase/* .

Each part can be both compiled to a runnable program, or a library linked by others. To compile a runnable program, goto that part and run command:

```sh
make intergrate # compile the runnable program
make clean # clean this directory and other dependencies
```

The dependencies are as follows, once again, each part can be run and tested independently.

a) lexser: itself

b) parser: lexser

c) interpreter: parser, lexser

d) compiler: parser, lexser

e) vm: compiler, parser, lexser

## DEMO ##

Let's take a simple program as an example, the program actually recusivly do arithmetic calucation of (4 + 3 + 2 + 1 = 10)

```CPP
function test(x) {
    function inner(x) {
        if (x > 1) {
            return inner(x - 1) + x;
        } else {
            return 1;
        }
    }
    return inner(x);
}
var res = test(4);
```

The output of parser is as follows, in fact it's the ast-tree of the previous program.

```txt
└─ <  > STATEMENTS
  ├─ < test > FUNCTION
  │ ├─ < x > IDENTIFIER
  │ └─ <  > STATEMENTS
  │   ├─ < inner > FUNCTION
  │   │ ├─ < x > IDENTIFIER
  │   │ └─ <  > STATEMENTS
  │   │   └─ < if > RESERVE
  │   │     ├─ < > > RESERVE
  │   │     │ ├─ < x > IDENTIFIER
  │   │     │ └─ < 1 > NUM
  │   │     ├─ <  > STATEMENTS
  │   │     │ └─ < return > RESERVE
  │   │     │   └─ < + > RESERVE
  │   │     │     ├─ < ( > FUNCTION
  │   │     │     │ ├─ < inner > IDENTIFIER
  │   │     │     │ └─ < - > RESERVE
  │   │     │     │   ├─ < x > IDENTIFIER
  │   │     │     │   └─ < 1 > NUM
  │   │     │     └─ < x > IDENTIFIER
  │   │     └─ <  > STATEMENTS
  │   │       └─ < return > RESERVE
  │   │         └─ < 1 > NUM
  │   └─ < return > RESERVE
  │     └─ < ( > FUNCTION
  │       ├─ < inner > IDENTIFIER
  │       └─ < x > IDENTIFIER
  └─ < var > RESERVE
    └─ < = > RESERVE
      ├─ < res > IDENTIFIER
      └─ < ( > FUNCTION
        ├─ < test > IDENTIFIER
        └─ < 4 > NUM
```

The interpreter will take the ast-tree as input, directly execute the the nodes of the ast-tree. Whenever an assign happens, the interpreter will output the assigned value, which can be the indications of running status.

output of interpreter, because there is only one assign:

```txt
assgin res 10
```

The compiler will try to compile the ast-tree, unlike interpreter, it will generate IR code, assembly code, and finally the executable code. Let's look at the IR code first. The IR code represents the purpose of ast-tree, and irrelevant to any specific cpu architecture.

```txt
.start:
        t0 := alloc res
        t1 := 4
        t1 := call func_start_test_1_1 t1 
        res := write t1
        halt

.func_start_test_1_1:
        t0 := alloc x
        t1 := read x
        t1 := call func_start_inner_1_2 t1 
        save t1
        jmp .func_end_test_1_1
.func_end_test_1_1:
        return

.func_start_inner_1_2:
        t0 := alloc x
.if_start_1:
        t1 := read x
        t2 := 1
        t1 := t1 > t2
        jcnz t1 .if_true_1
        jmp .if_false_1
.if_true_1:
        t1 := read x
        t2 := 1
        t1 := t1 - t2
        t1 := call func_start_inner_1_2 t1 
        t2 := read x
        t1 := t1 + t2
        save t1
        jmp .func_end_inner_1_2
        jmp .if_end_1
.if_false_1:
        t1 := 1
        save t1
        jmp .func_end_inner_1_2
.if_end_1:
.func_end_inner_1_2:
        return
```

Next the compiler will translate the IR code to arch-specific assembly code. Let's have a look. OK, the code is a little bit longer and difficult than IR. Here, stm is store to memory, ldm is load from memory.

```txt
.start:
        push ebp
        mov ebp, esp
        sub esp, 8
        stm ebp - 8, 4
        ldm r0, ebp - 8
        push r0
        call func_start_test_1_1
        add esp, 4
        stm ebp - 8, r0
        ldm r0, ebp - 8
        stm ebp - 4, r0
        add esp, 8
        pop ebp
        halt

.func_start_test_1_1:
        push ebp
        mov ebp, esp
        sub esp, 8
        ldm r0, ebp + 8
        stm ebp - 4, r0
        ldm r0, ebp - 4
        stm ebp - 8, r0
        ldm r0, ebp - 8
        push r0
        call func_start_inner_1_2
        add esp, 4
        stm ebp - 8, r0
        ldm r0, ebp - 8
        jmp .func_end_test_1_1
.func_end_test_1_1:
        add esp, 8
        pop ebp
        ret

.func_start_inner_1_2:
        push ebp
        mov ebp, esp
        sub esp, 12
        ldm r0, ebp + 8
        stm ebp - 4, r0
.if_start_1:
        ldm r0, ebp - 4
        stm ebp - 8, r0
        stm ebp - 12, 1
        ldm r0, ebp - 8
        ldm r1, ebp - 12
        cgt r0, r1
        stm ebp - 8, r0
        ldm r0, ebp - 8
        jcnz .if_true_1
        jmp .if_false_1
.if_true_1:
        ldm r0, ebp - 4
        stm ebp - 8, r0
        stm ebp - 12, 1
        ldm r0, ebp - 8
        ldm r1, ebp - 12
        sub r0, r1
        stm ebp - 8, r0
        ldm r0, ebp - 8
        push r0
        call func_start_inner_1_2
        add esp, 4
        stm ebp - 8, r0
        ldm r0, ebp - 4
        stm ebp - 12, r0
        ldm r0, ebp - 8
        ldm r1, ebp - 12
        add r0, r1
        stm ebp - 8, r0
        ldm r0, ebp - 8
        jmp .func_end_inner_1_2
        jmp .if_end_1
.if_false_1:
        stm ebp - 8, 1
        ldm r0, ebp - 8
        jmp .func_end_inner_1_2
.if_end_1:
.func_end_inner_1_2:
        add esp, 12
        pop ebp
        ret
```

Finally, the compiler will translate the assembly code into binary, that is, the executable code which specific cpu or vm can understand. The binary code is as follows. The instruction structure is borrowed from [linux ebpf](https://github.com/iovisor/bpf-docs/blob/master/eBPF.md), you can find more detail in instruction.h

```hexa
          0x9003
          0x89bf
     0x800000817
     0x4fff80962
      0xfff89061
             0x3
    0x7000000085
     0x400000807
      0xfff80963
      0xfff89061
      0xfffc0963
     0x800000807
           0x913
          ......
```

The vm will take the executable biary code as input, one instruction at a time, to decode and execute it. the vm's output is as follows. Each time the vm executing an instruction, it will decode and output the instruction, you can compare it with binary code mentioned previously. Since there are loops, function calls and recusion, the output will be much more. Each time a function returns, the vm will output the returned value, which can indicate the vm's running status.

```txt
             0x3                0              0x9                0                0
            0xbf              0x9              0x8                0                0
            0x17              0x8                0                0              0x8
            0x62              0x9                0       0xfffffff8              0x4
            0x61                0              0x9       0xfffffff8                0
             0x3                0                0                0                0
            0x85                0                0                0             0x70
             0x3                0              0x9                0                0
......
            0x95                0                0                0                0
return              0x3
             0x7              0x8                0                0              0x4
......
            0x95                0                0                0                0
return              0x6
             0x7              0x8                0                0              0x4
......
```
