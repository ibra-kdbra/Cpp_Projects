#include <iostream>
namespace simple_lexser{

const char *const reserved_list[] = {
    "auto",  // 0
    "break",  // 1
    "case",  // 2
    "char",  // 3
    "const",  // 4
    "continue",  // 5
    "default",  // 6
    "do",  // 7
    "double",  // 8
    "else",  // 9
    "enum",  // 10
    "extern",  // 11
    "float",  // 12
    "for",  // 13
    "goto",  // 14
    "if",  // 15
    "int",  // 16
    "long",  // 17
    "register",  // 18
    "return",  // 19
    "short",  // 20
    "signed",  // 21
    "sizeof",  // 22
    "static",  // 23
    "struct",  // 24
    "switch",  // 25
    "typedef",  // 26
    "union",  // 27
    "unsigned",  // 28
    "void",  // 29
    "volatile",  // 30
    "while",  // 31
    "-",  // 32
    "--",  // 33
    "-=",  // 34
    "->",  // 35
    "!",  // 36
    "!=",  // 37
    "%",  // 38
    "%=",  // 39
    "&",  // 40
    "&&",  // 41
    "&=",  // 42
    "(",  // 43
    ")",  // 44
    "*",  // 45
    "*=",  // 46
    ",",  // 47
    ".",  // 48
    "/",  // 49
    "/=",  // 50
    ":",  // 51
    ";",  // 52
    "?",  // 53
    "[",  // 54
    "]",  // 55
    "^",  // 56
    "^=",  // 57
    "{",  // 58
    "|",  // 59
    "||",  // 60
    "|=",  // 61
    "}",  // 62
    "~",  // 63
    "+",  // 64
    "++",  // 65
    "+=",  // 66
    "<",  // 67
    "<<",  // 68
    "<<=",  // 69
    "<=",  // 70
    "=",  // 71
    "==",  // 72
    ">",  // 73
    ">=",  // 74
    ">>",  // 75
    ">>=",  // 76
    "\"",  // 77

    "'",  // 78
    "\\",  // 78
    "~",  //79
    "function", //80
    "var", //81
};

std::string find_reserve_by_index(int32_t index)
{
    if (index < sizeof(reserved_list) / sizeof(char *)) {
        return std::string(reserved_list[index]);
    } else {
        return nullptr;
    }
}

int32_t find_index_by_reserve(std::string str)
{
    for (int i = 0; i < sizeof(reserved_list) / sizeof(char *); ++i) {
        if (reserved_list[i] == str) {
            return i;
        }
    }
    return -1;
}

}