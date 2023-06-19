
#include "hangman.h"


void word_init(char *string, strings strs, int e)
{
	int l = strlen(string);
	strs->o_string_uc = malloc(l);
	strs->m_string = malloc(l);
	strcpy(strs->o_string_uc, string);
	for (int i = 0; i < l; i++)
		strs->o_string_uc[i] = toupper(strs->o_string_uc[i]);
	strcpy(strs->m_string, strs->o_string_uc);
	for (int i = 0; i < l; i++)
	{
		strs->m_string[i] = toupper(strs->m_string[i]);
		if ((i > 0 && i < l -1) && isalpha(strs->m_string[i])
		    && (strs->m_string[i] != strs->m_string[0])
		    && (strs->m_string[i] != strs->m_string[l - 1]))
		  strs->m_string[i] = '-';
	}
}


void add_char(strings strs, char c)
{
	int l = strlen(strs->o_string_uc);
	int i = 0;

	while(i < l)
	{
		if(strs->o_string_uc[i] == c)
			strs->m_string[i] = c;
		i++;
	}
}

void word_set(strings strs, char c)
{
	if (strchr(strs->m_string, c))
		printf("character already exists in the string");
	else if (strchr(strs->o_string_uc, c))
		add_char(strs, c);
	else
	{
		printf("wrong answer\n");
		printf("number of attempts left : %d", strs->attempts);

		strs->attempts--;
	}
}

void looper(strings strs)
{
	char c;
	printf("|%s|\n", strs->m_string);
	while(1)
	{
		if (!strs->attempts)
		{
			print_ascii();
			printf("you lose!\n");
			printf("the word was : %s\n", strs->o_string_uc);
			break ;
		}
		if (!strcmp(strs->o_string_uc, strs->m_string))
		{
			printf("you win!");
			break ;
		}
		printf("insert a character: ");
		scanf(" %c", &c);
		word_set(strs, toupper(c));
		printf("\n|%s|\n", strs->m_string);
	}
}

void print_ascii(void)
{
		printf("_____\n");
		printf("|/   |\n");
		printf("|   (_)\n");
		printf("|   /|\\\n");
		printf("|    |\n");
		printf("|   | |\n");
		printf("|      \n");
		printf("|_____ \n");
		printf(" ");
}


int main(int argc, char **argv)
{
	S_strings strs;
	if (argc != 3)
	{
		printf("lol wrong input , RTFM!");
		return (0);
	}
	printf("hangman game you have %d attempts!\n", atoi(argv[2]));
	for (int i = 0; i < 100; i++)
		printf("\n");
	word_init(argv[1], &strs, atoi(argv[2]));
	strs.attempts = atoi(argv[2]);
	looper(&strs);
	free(strs.o_string_uc);
	free(strs.m_string);
	return (0);
}
