#include "../../inc/minishell.h"

bool	check_characters(const char *line, const char *charset)
{
	int		i;
	int		state;
	int		j;

	i = 0;
	state = 0;
	while (line[i])
	{
		chg_qte_state(&state, line[i]);
		j = 0;
		while (state == 0 && charset[j])
		{
			if (line[i] == charset[j])
			{
				ft_dprintf(2,
					"minishell: syntax error: `%c' in input\n", charset[j]);
				return (false);
			}
			j++;
		}
		i++;
	}
	return (true);
}

bool	check_pipe(const char *line)
{
	if (line[0] == '|')
	{
		ft_dprintf(2, "minishell: syntax error near unexpected token `|'\n");
		return (false);
	}
	return (true);
}
