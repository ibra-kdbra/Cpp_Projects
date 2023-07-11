#ifndef TOK_H
#define TOK_H

#include <string.h>
#include <stdlib.h>

/* token types */
enum { TOK_COMMENT, TOK_PRECOMPILER, TOK_OPERATOR, TOK_NUMBER, TOK_STRING, TOK_SEPARATOR, TOK_TYPE, TOK_KEYWORD,
    TOK_FUNCTION, TOK_VARIABLE };
typedef struct {
    int num;        /* number of tokens */
    char **tokens;  /* First character is a token type (see TOK_* enums), the rest is the string from the source code. */
} tok_t;

/* language rules is a two dimensional string array, each a NULL terminated list of matching patterns (lowercase only),
 * where the top dimension encodes the token's type. For example:
 *   char *c_comments[] =    { "\\/\\/.*?$", "\\/\\*.*?\\*\\/", NULL };
 *   char *c_precompiler[] = { "#.*?$", NULL };
 *   char *c_operators[] =   { "->", "[=\\<\\>\\+\\-\\*\\/%&\\^\\|!:\\.][=]?", NULL };
 *   char *c_numbers[] =     { "[0-9][0-9bx]?[0-9\\.a-f\\+\\-]*[UL]*", NULL };
 *   char *c_strings[] =     { "\"", "'", "L\"", "L'", NULL };
 *   char *c_separators[] =  { "[", "]", "{", "}", ",", ";", NULL };
 *   char *c_types[] =       { "signed", "unsigned", "char", "short", "int", "long", "float", "double", "true", NULL };
 *   char *c_keywords[] =    { "if", "else", "switch", "case", "for", "while", "do", "break", "continue", "return", NULL };
 *   char **c_rules[] =      { c_comments, c_precompiler, c_operators, c_numbers, c_strings, c_separators, c_types, c_keywords };
 *
 *   tok_new(&tok, c_rules, source_string, -1);
 */

int  tok_new(tok_t *tok, char ***rules, char *src, int len);/* create new token list from string according to language rules */
int  tok_tostr(tok_t *tok, char *dst, int maxlen);          /* convert token list to string */
int  tok_strlen(tok_t *tok);                                /* returns how big buffer is required for tok_tostr() */
int  tok_delete(tok_t *tok, int idx);                       /* remove a token */
int  tok_insert(tok_t *tok, int idx, char type, char *str); /* insert a token before idx */
int  tok_replace(tok_t *tok, int idx, char type, char *str);/* replace a token */
int  tok_append(tok_t *tok, char type, char *str);          /* append a token at the end of the list */
int  tok_find(tok_t *tok, int idx, char type, char *str);   /* find next occurance of either type or str */
int  tok_next(tok_t *tok, int idx, char type, char *str);   /* same as find but considers opening/closing parenthesis */
int  tok_match(tok_t *tok, int idx, int num, ...);          /* match a token pattern */
void tok_free(tok_t *tok);                                  /* free resources */
void tok_dump(tok_t *tok, int sidx, int eidx);              /* for debugging */

#ifdef TOK_IMPLEMENTATION

/* in case people want to override libc defaults */
#ifndef TOK_REALLOC
#define TOK_REALLOC realloc
#endif
#ifndef TOK_FREE
#define TOK_FREE free
#endif
#ifndef TOK_MEMSET
#define TOK_MEMSET memset
#endif
#ifndef TOK_MEMCPY
#define TOK_MEMCPY memcpy
#endif
#ifndef TOK_MEMMOVE
#define TOK_MEMMOVE memmove
#endif
#ifndef TOK_MEMCMP
#define TOK_MEMCMP memcmp
#endif
#ifndef TOK_STRLEN
#define TOK_STRLEN strlen
#endif
#ifndef TOK_STRCMP
#define TOK_STRCMP strcmp
#endif
#ifndef TOK_ATOI
#define TOK_ATOI atoi
#endif
#ifndef TOK_VA_LIST
#define TOK_VA_LIST __builtin_va_list
#endif
#ifndef TOK_VA_START
#define TOK_VA_START __builtin_va_start
#endif
#ifndef TOK_VA_END
#define TOK_VA_END __builtin_va_end
#endif
#ifndef TOK_VA_ARG
#define TOK_VA_ARG __builtin_va_arg
#endif

/* pad strings with this many additional zero bytes to avoid out of bound reads with look-ahead */
#ifndef TOK_PAD
#define TOK_PAD 4
#endif

/*** Private API ***/

/**
 * Check if a string is listed in an array
 */
static int _tok_in_array(char *s, char **arr)
{
    int i;

    if(!s || !*s || !arr) return 0;
    for(i = 0; arr[i]; i++)
        if(!TOK_STRCMP(s, arr[i])) return 1;
    return 0;
}

/**
 * A very minimalistic, non-UTF-8 aware regexp matcher. Enough to match language keywords.
 * Returns how many bytes matched, 0 if pattern doesn't match, -1 if pattern is bad.
 * Supports:
 * $      - matches end of line
 * .*?    - skip bytes until the following pattern matches
 *
 * [abc]  - one of the listed chars
 * [^abc] - all but the listed chars
 * [a-z]  - intervals, characters from 'a' to 'z'
 * [0-9]  - numbers (\d not supported)
 * a      - an exact char
 * .      - any char
 * \.     - match the dot char
 *
 * ?      - one or zero match
 * +      - at least one match
 * *      - any number of matches
 * {n}    - exactly n matches
 * {n,}   - at least n matches
 * {n,m}  - at least n, but no more than m matches
 */
static int _tok_regexp(char *regexp, char *str)
{
    unsigned char valid[256], *c=(unsigned char*)regexp, *s=(unsigned char *)str;
    int d, r, rmin, rmax, neg;
    if(!regexp || !regexp[0] || !str || !str[0]) return -1;
    while(*c) {
        if(*c == '(' || *c == ')') { c++; continue; }
        rmin = rmax = r = 1; neg = 0;
        TOK_MEMSET(valid, 0, sizeof(valid));
        /* special case, non-greedy match */
        if(c[0] == '.' && c[1] == '*' && c[2] == '?') {
            c += 3; if(!*c) return -1;
            if(*c == '$') { c++; while(*s && *s != '\n') s++; }
            else { while(*s && !_tok_regexp((char*)c, (char*)s)) s++; }
        } else {
            /* get valid characters list */
            if(*c == '\\') { c++; valid[(unsigned int)*c] = 1; } else {
                if(*c == '[') {
                    c++; if(*c == '^') { c++; neg = 1; }
                    while(*c && *c != ']') {
                        if(*c == '\\') { c++; valid[(unsigned int)*c] = 1; }
                        if(c[1] == '-') { for(d = *c, c += 2; d <= *c; d++) valid[d] = 1; }
                        else valid[(unsigned int)(*c == '$' ? 10 : *c)] = 1;
                        c++;
                    }
                    if(!*c) return -1;
                    if(neg) { for(d = 0; d < 256; d++) valid[d] ^= 1; }
                }
                else if(*c == '.') { for(d = 0; d < 256; d++) valid[d] = 1; }
                else valid[(unsigned int)(*c == '$' ? 10 : *c)] = 1;
            }
            c++;
            /* make it case-insensitive */
            for(d = 0; d < 26; d++) {
                if(valid[d + 'a']) valid[d + 'A'] = 1; else
                if(valid[d + 'A']) valid[d + 'a'] = 1;
            }
            /* get repeat count */
            if(*c == '{') {
                c++; rmin = TOK_ATOI((char*)c); rmax = 0; while(*c && *c != ',' && *c != '}') c++;
                if(*c == ',') { c++; if(*c != '}') { rmax = TOK_ATOI((char*)c); while(*c && *c != '}') c++; } }
                if(*c != '}') return -1;
                c++;
            }
            else if(*c == '?') { c++; rmin = 0; rmax = 1; }
            else if(*c == '+') { c++; rmin = 1; rmax = 0; }
            else if(*c == '*') { c++; rmin = 0; rmax = 0; }
            /* do the match */
            for(r = 0; *s && valid[(unsigned int)*s] && (!rmax || r < rmax); s++, r++);
        }
        if((!*s && *c) || r < rmin) return 0;
    }
    return (int)((intptr_t)s - (intptr_t)str);
}

/*** Public API ***/

/**
 * Source code tokenizer.
 * @param tok: tok instance
 * @param src: zero terminated UTF-8 string
 * @param maxlen: length of the string or -1
 * @return 1 on success, 0 on failure
 */
int tok_new(tok_t *tok, char ***rules, char *src, int len)
{
    char *s, *d, ***r = rules;
    int i, j, k, l, m, at = 0, nt = 0, *t = NULL;

    if(len == -1 && src) len = TOK_STRLEN(src);
    if(tok && rules && src && *src && len > 0) {
        TOK_MEMSET(tok, 0, sizeof(tok_t));
        /* tokenize string */
        for(k = 0; src[k] && k < len; ) {
            if(nt + 2 >= at) {
                t = (int*)TOK_REALLOC(t, (at + 256) * sizeof(int));
                if(!t) return 0;
                TOK_MEMSET(t + at, 0, 256 * sizeof(int));
                at += 256;
            }
            if(src[k] == '(') { t[nt++] = (k << 4) | 5; k++; continue; }
            j = 0; if(r[5]) for(i = 0; r[5][i]; i++) if(r[5][i][0] == src[k]) { j = 1; break; }
            if(src[k] == ')' || src[k] == ' ' || src[k] == '\t' || src[k] == '\r' || src[k] == '\n' || j) {
                if(!nt || (t[nt - 1] & 0xf) != 5 || src[k] == ',') t[nt++] = (k << 4) | 5;
                k++; continue;
            }
            for(m = 0; m < 4; m++)
                if(r[m])
                    for(i = 0; r[m][i]; i++) {
                        l = _tok_regexp(r[m][i], src + k);
                        if(l > 0) {
                            if(!nt || (t[nt - 1] & 0xf) != m) t[nt++] = (k << 4) | m;
                            k += l - 1; goto nextchar;
                        }
                    }
            if(r[4])
                for(i = 0; r[4][i]; i++) {
                    l = TOK_STRLEN(r[4][i]);
                    if(!TOK_MEMCMP(src + k, r[4][i], l)) {
                        if(!nt || (t[nt - 1] & 0xf) != 4) t[nt++] = (k << 4) | 4;
                        for(k += l; src[k]; k++) {
                            if(src[k] == '\\') k++; else
                            if(src[k] == r[4][i][l - 1]) { if(src[k + 1] != r[4][i][l - 1]) break; else k++; }
                        }
                        goto nextchar;
                    }
                }
            if(!nt || (t[nt - 1] & 0xf) != 9) t[nt++] = (k << 4) | 9;
nextchar:   if(src[k]) k++;
        }
        if(t) {
            for(i = 0; i < nt; i++) {
                if((t[i] & 0xf) == 9) {
                    j = i + 1 < nt ? t[i + 1] >> 4 : k;
                    l = t[i] >> 4;
                    s = d = (char*)TOK_REALLOC(NULL, j - l + 1);
                    if(!s) return 0;
                    for(; l < j; l++)
                        *d++ = src[l] >= 'A' && src[l] <= 'Z' ? src[l] + 'a' - 'A' : src[l];
                    *d = 0;
                    if(_tok_in_array(s, r[6])) t[i] = (t[i] & ~0xf) | 6; else
                    if(_tok_in_array(s, r[7])) t[i] = (t[i] & ~0xf) | 7; else
                    if(src[l] == '(' || (i + 2 < nt && (t[i + 2] & 0xf) == 5 && src[t[i + 2] >> 4] == '('))
                        t[i] = (t[i] & ~0xf) | 8;
                    TOK_FREE(s);
                }
                if(i && (t[i] & 0xf) == 3 && (t[i - 1] & 0xf) == 2 && (src[t[i - 1] >> 4] == '-' || src[t[i - 1] >> 4] == '.'))
                    t[i] -= 16;
            }
            tok->tokens = (char**)TOK_REALLOC(NULL, (nt + 1) * sizeof(char*));
            if(!tok->tokens) return 0;
            TOK_MEMSET(tok->tokens, 0, (nt + 1) * sizeof(char*));
            for(i = l = m = 0; i < nt; i++) {
                j = i + 1 < nt ? t[i + 1] >> 4 : k;
                if(j <= l) continue;
                tok->tokens[m] = (char*)TOK_REALLOC(NULL, j - l + TOK_PAD + 1);
                if(tok->tokens[m]) {
                    TOK_MEMSET(tok->tokens[m], 0, j - l + TOK_PAD + 1);
                    tok->tokens[m][0] = t[i] & 0xF;
                    TOK_MEMCPY(tok->tokens[m] + 1, src + l, j - l);
                    m++;
                }
                l = j;
            }
            tok->num = m;
            return 1;
        }
    }
    return 0;
}

/**
 * Detokenizer, constructs string from list of tokens
 * @param tok: tok instance
 * @param dst: destination buffer
 * @param maxlen: destination buffer's length (see tok_strlen())
 * @return size of the constructed string, or -1 if buffer wasn't big enough
 */
int tok_tostr(tok_t *tok, char *dst, int maxlen)
{
    char *end = dst + maxlen, *d = dst;
    int i, l;

    if(tok && tok->num > 0 && tok->tokens && dst && maxlen > 0)
        for(i = 0; i < tok->num; i++) {
            if(tok->tokens[i]) {
                l = TOK_STRLEN(tok->tokens[i] + 1);
                if(d + l < end) {
                    TOK_MEMCPY(d, tok->tokens[i] + 1, l);
                    d += l;
                } else
                    return -1;
            }
        }
    return (int)((uintptr_t)d - (uintptr_t)dst);
}

/**
 * Returns how big destination buffer is required for detokenization.
 * @param tok: tok instance
 * @return size of the constructed string plus one (for the zero terminator)
 */
int tok_strlen(tok_t *tok)
{
    int i, l = 0;

    if(tok && tok->num > 0 && tok->tokens)
        for(i = 0, l++; i < tok->num; i++)
            if(tok->tokens[i])
                l += TOK_STRLEN(tok->tokens[i] + 1);
    return l;
}

/**
 * Remove a token from list
 * @param tok: tok instance
 * @param idx: from where
 * @return 1 on success, 0 on failure
 */
int tok_delete(tok_t *tok, int idx)
{
    if(!tok || !tok->tokens || idx < 0 || idx >= tok->num) return 0;
    if(tok->tokens[idx]) TOK_FREE(tok->tokens[idx]);
    TOK_MEMCPY(&tok->tokens[idx], &tok->tokens[idx + 1], (tok->num - idx) * sizeof(char*));
    tok->num--;
    tok->tokens = (char**)TOK_REALLOC(tok->tokens, tok->num * sizeof(char*));
    if(!tok->tokens) { tok->num = 0; return 0; }
    return 1;
}

/**
 * Insert a token into the list before the given index
 * @param tok: tok instance
 * @param idx: where (-1 at the end)
 * @param type: one of TOK_* enums
 * @param str: zero terminated UTF-8 string
 * @return 1 on success, 0 on failure
 */
int tok_insert(tok_t *tok, int idx, char type, char *str)
{
    int l;
    char *dst;

    if(idx == -1 || idx == tok->num) return tok_append(tok, type, str);
    if(!tok || !tok->tokens || idx < 0 || idx > tok->num || type < 0 || type > 9 || !str || !*str) return 0;
    l = TOK_STRLEN(str);
    tok->tokens = (char**)TOK_REALLOC(tok->tokens, (tok->num + 1) * sizeof(char*));
    if(!tok->tokens) { tok->num = 0; return 0; }
    dst = (char*)TOK_REALLOC(NULL, l + TOK_PAD + 1);
    if(!dst) return 0;
    TOK_MEMSET(dst, 0, l + TOK_PAD + 1);
    TOK_MEMCPY(dst + 1, str, l);
    dst[0] = type;
    TOK_MEMMOVE(&tok->tokens[idx + 1], &tok->tokens[idx], (tok->num - idx) * sizeof(char*));
    tok->tokens[idx] = dst;
    tok->num++;
    return 1;
}

/**
 * Replace a token
 * @param tok: tok instance
 * @param idx: where
 * @param type: one of TOK_* enums
 * @param str: zero terminated UTF-8 string
 * @return 1 on success, 0 on failure
 */
int tok_replace(tok_t *tok, int idx, char type, char *str)
{
    int l;
    char *dst;

    if(!tok || !tok->tokens || idx < 0 || idx >= tok->num || type < 0 || type > 9 || !str || !*str) return 0;
    l = TOK_STRLEN(str);
    dst = (char*)TOK_REALLOC(NULL, l + TOK_PAD + 1);
    if(!dst) return 0;
    TOK_MEMSET(dst, 0, l + TOK_PAD + 1);
    TOK_MEMCPY(dst + 1, str, l);
    dst[0] = type;
    if(tok->tokens[idx]) TOK_FREE(tok->tokens[idx]);
    tok->tokens[idx] = dst;
    return 1;
}

/**
 * Append a token at the end of list
 * @param tok: tok instance
 * @param type: one of TOK_* enums
 * @param str: zero terminated UTF-8 string
 * @return 1 on success, 0 on failure
 */
int tok_append(tok_t *tok, char type, char *str)
{
    int l;
    char *dst;

    if(!tok || !tok->tokens || type < 0 || type > 9 || !str || !*str) return 0;
    tok->tokens = (char**)TOK_REALLOC(tok->tokens, (tok->num + 1) * sizeof(char*));
    if(!tok->tokens) { tok->num = 0; return 0; }
    l = TOK_STRLEN(str);
    dst = (char*)TOK_REALLOC(NULL, l + TOK_PAD + 1);
    if(!dst) return 0;
    TOK_MEMSET(dst, 0, l + TOK_PAD + 1);
    TOK_MEMCPY(dst + 1, str, l);
    dst[0] = type;
    tok->tokens[tok->num++] = dst;
    return 1;
}

/**
 * Find a token
 * @param tok: tok instance
 * @param idx: starting from here
 * @param type: one of TOK_* enums or -1 (any token)
 * @param str: zero terminated UTF-8 string or NULL (any string)
 * @return index of first occurance or -1 if not found
 */
int tok_find(tok_t *tok, int idx, char type, char *str)
{
    int i;

    if(!tok || !tok->tokens || idx < 0 || idx >= tok->num || type < -1 || type > 9) return -1;
    for(i = idx; i < tok->num; i++)
        if(tok->tokens[i] && ((type == -1 || tok->tokens[i][0] == type) && (str == NULL || !TOK_STRCMP(tok->tokens[i] + 1, str))))
            return i;
    return -1;
}

/**
 * Find a token, but consider parenthesis
 * @param tok: tok instance
 * @param idx: starting from here
 * @param type: one of TOK_* enums or -1 (any token)
 * @param str: zero terminated UTF-8 string or NULL (any string)
 * @return index of first occurance at the same parenthesis level or -1 if not found
 */
int tok_next(tok_t *tok, int idx, char type, char *str)
{
    int i, j, p = 0, s = 0, b = 0;

    if(!tok || !tok->tokens || idx < 0 || idx >= tok->num || type < -1 || type > 9) return -1;
    for(i = idx; i < tok->num; i++)
        if(tok->tokens[i]) {
            /* skip string literals */
            if(tok->tokens[i][0] != 4) {
                for(j = 1; tok->tokens[i][j]; j++) {
                    if(tok->tokens[i][j] == '(') p++;
                    if(tok->tokens[i][j] == ')') p--;
                    if(tok->tokens[i][j] == '{') s++;
                    if(tok->tokens[i][j] == '}') s--;
                    if(tok->tokens[i][j] == '[') b++;
                    if(tok->tokens[i][j] == ']') b--;
                }
            }
            if(!p && !s && !b && ((type == -1 || tok->tokens[i][0] == type) && (str == NULL || !TOK_STRCMP(tok->tokens[i] + 1, str))))
                return i;
        }
    return -1;
}

/**
 * Match a token pattern
 * @param tok: tok instance
 * @param idx: starting from here
 * @param num: number of token types to match
 * @param ...: token types, one of the TOK_* enums
 * @return 1 if matches, 0 otherwise
 */
int tok_match(tok_t *tok, int idx, int num, ...)
{
    int i;
    TOK_VA_LIST args;
    TOK_VA_START(args, num);

    if(!tok || !tok->tokens || idx < 0 || num < 1 || idx + num >= tok->num) { TOK_VA_END(args); return 0; }
    for(i = idx; i < idx + num; i++)
        if(!tok->tokens[i] || (int)tok->tokens[i][0] != TOK_VA_ARG(args, int)) { TOK_VA_END(args); return 0; }
    TOK_VA_END(args);
    return 1;
}

/**
 * Free token list
 * @param tok: tok instance
 */
void tok_free(tok_t *tok)
{
    int i;

    if(tok && tok->num > 0 && tok->tokens)
        for(i = 0; i < tok->num; i++)
            if(tok->tokens[i]) TOK_FREE(tok->tokens[i]);
    TOK_MEMSET(tok, 0, sizeof(tok_t));
}

#ifndef TOK_NODEBUG
/**
 * Dump token list (for debugging)
 * @param tok: tok instance
 * @param sidx: starting from this index (use 0 to dump all)
 * @param eidx: ending with this index (use 0 to dump all)
 */
void tok_dump(tok_t *tok, int sidx, int eidx)
{
    int i;
    static char *types[] = { "TOK_COMMENT    ", "TOK_PRECOMPILER", "TOK_OPERATOR   ", "TOK_NUMBER     ", "TOK_STRING     ",
          "TOK_SEPARATOR  ", "TOK_TYPE       ", "TOK_KEYWORD    ", "TOK_FUNCTION   ", "TOK_VARIABLE   ", "???            " };
    if(tok) {
        printf("tok.num %u\r\n", tok->num);
        if(sidx < 0) sidx = 0;
        if(eidx < 1 || eidx > tok->num) eidx = tok->num;
        if(tok->num > 0 && tok->tokens)
            for(i = sidx; i < eidx; i++)
                printf("tok.tokens[%04u] %x %s '%s'\r\n", i, tok->tokens[i] ? tok->tokens[i][0] : 0xff,
                    types[(int)(tok->tokens[i] && tok->tokens[i][0] >= 0 && tok->tokens[i][0] < 10 ? tok->tokens[i][0] : 10)],
                    tok->tokens[i] ? tok->tokens[i] + 1 : "(NULL)");
    }
}
#endif

#endif /* TOK_IMPLEMENTATION */

#endif /* TOK_H */
