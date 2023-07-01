#include "../../inc/minishell.h"

void	chg_qte_state(int *state, int c)
{
	if (c == '\'')
	{
		if (*state == 1)
			*state = 0;
		else if (*state == 0)
			*state = 1;
	}
	if (c == '\"')
	{
		if (*state == 2)
			*state = 0;
		else if (*state == 0)
			*state = 2;
	}
}
