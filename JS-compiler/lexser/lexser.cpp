#include "reserved_list.h"
#include <assert.h>
#include <cstring>
#include <cstdio>
#include "token.h"
#include "lexser.h"
#include <vector>
using namespace std;
using namespace simple_lexser;

static bool is_num(const char *p)
{
    return (*p >= '0') && (*p <= '9');
}

static bool is_char(const char *p)
{
    return ((*p >= 'a') && (*p <= 'z')) || ((*p >= 'A') && (*p <= 'Z'));
}

static bool is_blank(const char *p)
{
    return (*p == ' ') || (*p == '\t');
}

static string split_to_string(const char *start, const char *end)
{
    assert(start <= end);
    char buf[end - start + 1];
    strncpy(buf, start, end - start);
    buf[end - start] = '\0';
    return string(buf);
}

Token_t *Lexser::get_token(const char *curser, int32_t *skip)
{
    if (is_blank(curser)) {
        int i;
        for (i = 0; is_blank(curser + i); ++i);
        *skip = i;
        return nullptr;
    }
    if (is_num(curser)) {   //num check
        int i;
        for (i = 0; is_num(curser + i); ++i);
        string value = split_to_string(curser, curser + i);
        *skip = i;
        ++index;
        return new Token_t(line, index, value, NUM);
    }
    if (is_char(curser)) {  //identifier and reserved keyword check
        int i;
        for (i = 0; is_char(curser + i) || is_num(curser + i) || *(curser + i) == '_'; ++i);
        string value = split_to_string(curser, curser + i);
        *skip = i;
        ++index;
        if (simple_lexser::find_index_by_reserve(value) >= 0) {    //reserved
            return new Token_t(line, index, value, RESERVE);
        } else {                                    //identifier
            return new Token_t(line, index, value, IDENTIFIER);
        }
    }
    switch (*curser)
    {
        case '\n': {
            ++line;
            index = 0;
            *skip = 1;
            return nullptr;
        }
        case '"': { //scan until '"', skip "\"", but keep "\\"", very disturbing
            int i;
            for (i = 1; *(curser + i) != '\0'; ++i) {
                if (*(curser + i) == '"') {  
                    int count = 0;
                    for (int j = i - 1; *(curser + j) == '\\'; --j, ++count);
                    if (count % 2 == 0) {
                        *skip = i + 1;
                        ++index;
                        return new Token_t(line, index, split_to_string(curser + 1, curser + i), STRING);
                    }
                }
            }
            printf("not pair \" line %d, token index %d\n", line, index);
            exit(-1);  
        }
        case '\'': { // '\' '\'' '\\'
            int  i;
            for (i = 1; *(curser + i) != '\0' && i < 4; ++i) {
                if (*(curser + i) == '\'') {
                    if (i == 1) { // ''
                        printf("empty \'\' line %d, token index %d\n", line, index);
                        exit(-1);                         
                    } else {
                        int count = 0;
                        for (int j = i - 1; *(curser + j) == '\\'; --j, ++count);
                        if (count % 2 == 0) {
                            *skip = i + 1;
                            ++index;
                            return new Token_t(line, index, split_to_string(curser + 1, curser + i), STRING);
                        }
                    }
                }
            }
            printf("not pair \' line %d, token index %d\n", line, index);
            exit(-1); 
        }
        case '-': {
            switch (*(curser + 1)) {
                case '-':
                case '=':
                case '>':
                    goto len2;
                default:
                    goto len1;
            }
        }
        case '+': {
            switch (*(curser + 1)) {
                case '+':
                case '=':
                    goto len2;
                default:
                    goto len1;
            }            
        }
        case '&': {
            switch (*(curser + 1)) {
                case '&':
                case '=':
                    goto len2;
                default:
                    goto len1;
            }            
        }
        case '|': {
            switch (*(curser + 1)) {
                case '|':
                case '=':
                    goto len2;
                default:
                    goto len1;
            }            
        }
        case '<': {
            switch (*(curser + 1)) {
                case '<': {
                    if (*(curser + 2) == '=')
                        goto len3;
                    else
                        goto len2;
                }
                case '=':
                    goto len2;
                default:
                    goto len1;
            }            
        }
        case '>': {
            switch (*(curser + 1)) {
                case '>': {
                    if (*(curser + 2) == '=')
                        goto len3;
                    else
                        goto len2;
                }
                case '=':
                    goto len2;
                default:
                    goto len1;
            }            
        }
        case '_': {
            int i;
            for (i = 0; *(curser + i) == '_' || is_num(curser + i)|| 
                is_char(curser + i); ++i);
            *skip = i;
            return new Token_t(line, index, split_to_string(curser, curser + i), IDENTIFIER);
        }

        case '%':
        case '*':
        case '/':
        case '=':
        case '^':
        case '!': {
            switch(*(curser + 1)) {
                case '=':
                    goto len2;
                default:
                    goto len1;
            }
        }

        case '(':
        case ')':
        case '.':
        case ',':
        case ':':
        case ';':
        case '?':
        case '[':
        case ']':
        case '{':
        case '}':
        case '~': 
        case '\\':
len1:
        *skip = 1;
        ++index;
        return new Token_t(line, index, split_to_string(curser, curser + 1), RESERVE);
len2:
        *skip = 2;
        ++index;
        return new Token_t(line, index, split_to_string(curser, curser + 2), RESERVE);
len3:
        *skip = 3;
        ++index;
        return new Token_t(line, index, split_to_string(curser, curser + 3), RESERVE);

        default: //correct char should never reach here
            printf("unknown char %c at line %d, token index %d\n", *curser, line, index);
            exit(-1);
    }
}

void Lexser::lex(const char *src)
{
    const char *curser = src;
    int32_t skip;
    while (*curser != '\0') {
        Token_t *token = get_token(curser, &skip);
        if (token) {
            token_list.push_back(*token);
            delete(token);
        }
        curser += skip;
    }
    ++index;
    token_list.push_back(Token_t(line, index, string(), END));
}

Token_t& Lexser::get_token_by_index(int index)
{
    assert(index >= 0 && index < token_list.size());
    return token_list[index];
}
