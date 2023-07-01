#include "../../inc/minishell.h"

bool	check_for_double_operators(int *cmd_type)
{
	int		i;

	i = 0;
	while (cmd_type[i])
	{
		if (cmd_type[i] == PIPE && cmd_type[i + 1] == PIPE)
		{
			ft_dprintf(2, "minishell: syntax error near unexpected token `|'\n");
			return (false);
		}
		i++;
	}
	return (true);
}
