#include "../../inc/minishell.h"

bool	is_pipe(char c)
{
	if (c == '|')
		return (true);
	return (false);
}

bool	is_redirection(char c)
{
	if (c == '<' || c == '>')
		return (true);
	return (false);
}

bool	is_space(char c)
{
	if (c == ' ')
		return (true);
	return (false);
}
