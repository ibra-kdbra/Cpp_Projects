Tokenizer
=========

First, this `Tok` library has nothing to do with the Tok'ra. :-) Instead it is a really simple, dependency-free, single-header *source code tokenizer* written in ANSI C. It features an easy-to-use interface for manipulating tokens and concatenating them back into a string.

## Table of Contents

- [Usage](#usage)
- [Language Rules](#language-rules)
- [Tokens](#tokens)
- [API](#api)
    - [Tokenizer](#tokenizer)
    - [Detokenizer](#detokenizer)
    - [String Length](#string-length)
    - [Delete](#delete)
    - [Insert](#insert)
    - [Replace](#replace)
    - [Append](#append)
    - [Find](#find)
    - [Find Parenthesis Correctly](#find-parenthesis-correctly)
    - [Match Pattern](#match-pattern)
    - [Free](#free)
    - [Debugging](#debugging)


Usage
-----

Just include `tok.h`. In *exactly one* of your source files, also define the implementation:

```c
#define TOK_IMPLEMENTATION
#include "tok.h"
```

Normally it only uses libc, but you can also provide your own functions (and with that, remove even the libc dependency)
by overriding the function names with defines, like `TOK_REALLOC`, `TOK_STRLEN`, `TOK_MEMCPY`, `TOK_VA_LIST` etc.

Language Rules
--------------

Tok accepts language rules in a two dimensional string array, where each element is a `NULL` terminated list of simple regexp
patterns (lowercase only, the match is case in-sensitive), and where the top dimension encodes the token's type. For example:

```c
char *c_comments[] =    { "\\/\\/.*?$", "\\/\\*.*?\\*\\/", NULL };
char *c_precompiler[] = { "#.*?$", NULL };
char *c_operators[] =   { "->", "[=\\<\\>\\+\\-\\*\\/%&\\^\\|!:\\.][=]?", NULL };
char *c_numbers[] =     { "[0-9][0-9bx]?[0-9\\.a-f\\+\\-]*[UL]*", NULL };
char *c_strings[] =     { "\"", "'", "L\"", "L'", NULL };
char *c_separators[] =  { "[", "]", "{", "}", ",", ";", NULL };
char *c_types[] =       { "signed", "unsigned", "char", "short", "int", "long", "float", "double", "true", NULL };
char *c_keywords[] =    { "if", "else", "switch", "case", "for", "while", "do", "break", "continue", "return", NULL };

char **c_rules[] =      { c_comments, c_precompiler, c_operators, c_numbers, c_strings, c_separators, c_types, c_keywords };
```

You can then use this `c_rules` language rules set as:

```c
tok_t tok;

tok_new(&tok, c_rules, source_string, -1);
tok_dump(&tok);
tok_free(&tok);
```

Note this is just an example, and not a full list of the ANSI C language. If you provide a non-complete rules set, then the
worst that could happen is that some of the language specific keywords or operators are reported as a variable.

Tokens
------

There are `tok.num` tokens, and those are stored in a plain simple string array in `tok.tokens`. Here the first character is
always a token type, the rest is the string from the source code. K.I.S.S. The available token types are:

| Enum Type         | Description                                                                |
|-------------------|----------------------------------------------------------------------------|
| `TOK_COMMENT`     | A comment, could be multiline if you have used such a pattern              |
| `TOK_PRECOMPILER` | A precompiler directive                                                    |
| `TOK_OPERATOR`    | An operator                                                                |
| `TOK_NUMBER`      | A number literal                                                           |
| `TOK_STRING`      | A string literal                                                           |
| `TOK_SEPARATOR`   | A separator. Any whitespace plus the elements you specify to be separators |
| `TOK_TYPE`        | A language built-in variable type                                          |
| `TOK_KEYWORD`     | A language keyword                                                         |
| `TOK_FUNCTION`    | A function name (variable that's followed by `(`)                          |
| `TOK_VARIABLE`    | A variable                                                                 |

Note that the last two aren't part of the rules set, those are detected automatically. To access tokens one can do:

```c
if (tok.tokens[i][0] == TOK_KEYWORD)
    printf("The %uth token is '%s' and it is a keyword.\n", i, tok.tokens[i] + 1);
```

HINT: you can use `tok_dump()` to dump all the parsed tokens to stdout.

API
---

### Tokenizer

```c
tok_t tok;

int tok_new(tok_t *tok, char ***rules, char *src, int len);
```

Reads in a source code from a string, and fills in the tokenizer's structure.

| Argument | Description                                                    |
|----------|----------------------------------------------------------------|
| `tok`    | The Tok instance                                               |
| `rules`  | The language rules set to be used                              |
| `src`    | Zero terminated UTF-8 source string                            |
| `len`    | Length of string if it's not zero terminated, otherwise `-1`   |

Returns 1 on success, 0 otherwise.

### Detokenizer

```c
int tok_tostr(tok_t *tok, char *dst, int maxlen);
```

Constructs a string from list of tokens.

| Argument | Description                                                    |
|----------|----------------------------------------------------------------|
| `tok`    | The Tok instance                                               |
| `dst`    | Destination buffer                                             |
| `maxlen` | Destination buffer's length (see `tok_strlen()` below)         |

Returns the size of the constructed string (and the string in `dst`), or -1 if the destination buffer wasn't big enough.

### String Length

```c
int tok_strlen(tok_t *tok);
```

Returns how big destination buffer is required for `tok_tostr()` detokenization.

| Argument | Description                                                    |
|----------|----------------------------------------------------------------|
| `tok`    | The Tok instance                                               |

Returns the size of the constructed string plus one (for the zero terminator).

### Delete

```c
int tok_delete(tok_t *tok, int idx);
```

Removes the token specified by index `idx`. Tokens after that are moved forward.

| Argument | Description                                                    |
|----------|----------------------------------------------------------------|
| `tok`    | The Tok instance                                               |
| `idx`    | Index of the token                                             |

Returns 1 on success, 0 otherwise.

### Insert

```c
int tok_insert(tok_t *tok, int idx, char type, char *str);
```

Inserts a token into the list **before** the given index `idx`. Calling with `idx` being -1 or `tok.num` is the same as
calling `tok_append()`.

| Argument | Description                                                    |
|----------|----------------------------------------------------------------|
| `tok`    | The Tok instance                                               |
| `idx`    | Where to insert (use `-1` to insert at the end)                |
| `type`   | One of the `TOK_*` type enums                                  |
| `str`    | A zero terminated UTF-8 string                                 |

Returns 1 on success, 0 otherwise.

### Replace

```c
int tok_replace(tok_t *tok, int idx, char type, char *str);
```

Replaces a token at the given index `idx`.

| Argument | Description                                                    |
|----------|----------------------------------------------------------------|
| `tok`    | The Tok instance                                               |
| `idx`    | Index of the token                                             |
| `type`   | One of the `TOK_*` type enums                                  |
| `str`    | A zero terminated UTF-8 string                                 |

Returns 1 on success, 0 otherwise.

### Append

```c
int tok_append(tok_t *tok, char type, char *str);
```

Appends a token at the end of the token list.

| Argument | Description                                                    |
|----------|----------------------------------------------------------------|
| `tok`    | The Tok instance                                               |
| `type`   | One of the `TOK_*` type enums                                  |
| `str`    | A zero terminated UTF-8 string                                 |

Returns 1 on success, 0 otherwise.

### Find

```c
int tok_find(tok_t *tok, int idx, char type, char *str);
```

Looks for a token starting from index `idx`. It can either match `type` (if that's not -1) or `str` (if that's not `NULL`)
or both. This function does not care about parenthesis.

| Argument | Description                                                    |
|----------|----------------------------------------------------------------|
| `tok`    | The Tok instance                                               |
| `idx`    | Index of the token                                             |
| `type`   | One of the `TOK_*` type enums or `-1` (any token)              |
| `str`    | A zero terminated UTF-8 string or `NULL` (any string)          |

Returns the index of the first occurrence of the token or -1 if not found.

### Find Parenthesis Correctly

```c
int tok_next(tok_t *tok, int idx, char type, char *str);
```

Looks for a token starting from index `idx`. It can either match `type` (if that's not -1) or `str` (if that's not `NULL`)
or both. It differs from `tok_find()` in a way that it considers parenthesis. For example, if we have a tokenlist
`(`, `a`, `,`, `b`, `)`, `,`, `c` then `tok_find(tok,0,-1,",")` returns 2, but `tok_next(tok,0,-1,",")` returns 5.

| Argument | Description                                                    |
|----------|----------------------------------------------------------------|
| `tok`    | The Tok instance                                               |
| `idx`    | Index of the token                                             |
| `type`   | One of the `TOK_*` type enums or `-1` (any token)              |
| `str`    | A zero terminated UTF-8 string or `NULL` (any string)          |

Returns the index of the first occurrence of the token at the same parenthesis level or -1 if not found.

### Match Pattern

```c
int tok_match(tok_t *tok, int idx, int num, ...);
```

Matches a token pattern starting from index `idx`.

| Argument | Description                                                    |
|----------|----------------------------------------------------------------|
| `tok`    | The Tok instance                                               |
| `idx`    | Index of the token                                             |
| `num`    | Number of tokens to match                                      |
| `...`    | List of `TOK_*` type enums                                     |

Returns 1 if the pattern matches, 0 otherwise.

### Free

```c
void tok_free(tok_t *tok);
```

Free the token list and clear instance to zero.

| Argument | Description                                                    |
|----------|----------------------------------------------------------------|
| `tok`    | The Tok instance                                               |

### Debugging

```c
void tok_dump(tok_t *tok, int sidx, int eidx);
```

This function is only available if the `TOK_NODEBUG` define isn't defined. It simply dumps the tokens to stdout using printf.

| Argument | Description                                                    |
|----------|----------------------------------------------------------------|
| `tok`    | The Tok instance                                               |
| `sidx`   | Starting from this index (inclusive, use `0` to dump all)      |
| `eidx`   | Ending with this index (exclisive, use `0` to dump all)        |
