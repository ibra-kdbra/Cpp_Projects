#ifndef HANGMAN_H
# define HANGMAN_H
# include <stdio.h>
# include <stdlib.h>
# include <ctype.h>
# include <string.h>

typedef struct _strings* strings;
typedef struct _strings S_strings;

struct _strings
{
	char *o_string_uc; //original input string upper cased
	char *m_string; //modified input string
    int attempts;
};
/*
** funciton that allocated and generate both the o_string_u
** and the m_string
**/
void word_init(char *string, strings strs, int e);
/* function that add a character to the m_string*/
void add_char(strings strs, char c);
/*function that call add_char when it's right to add a character to m_string*/
void word_set(strings strs, char c);
/** 
** function that keeps calling for word_set as long as we still have enough attempts
** and we didn't win the game yet
*/
void looper(strings strs);
/* function that prints a hanged man ascii art*/
void print_ascii(void);
#endif