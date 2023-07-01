#include "../../inc/minishell.h"

static char	*apply_delete(char *tok, int *i)
{
	char	*tmp[2];
	char	*new_tok;
	char	quote;
	int		j;

	quote = tok[*i];
	j = *i + 1;
	while (tok[j] != quote)
		j++;
	tmp[0] = ft_substr(tok, 0, *i);
	tmp[1] = ft_substr(tok, *i + 1, j - *i - 1);
	new_tok = ft_strjoin(tmp[0], tmp[1]);
	free (tmp[0]);
	free (tmp[1]);
	*i = j - 2;
	tmp[0] = ft_strjoin(new_tok, tok + j + 1);
	free (tok);
	free (new_tok);
	tok = ft_strdup(tmp[0]);
	free (tmp[0]);
	return (tok);
}

static char	*delete_quote_tok(char *tok)
{
	int		i;

	i = 0;
	while (tok[i])
	{
		if (tok[i] == '\'' || tok[i] == '"')
			tok = apply_delete(tok, &i);
		i++;
	}
	return (tok);
}

void	remove_quote(char **cmd)
{
	int	i;

	i = 0;
	while (cmd[i])
	{
		if (((cmd[i] && ft_strchr(cmd[i], '\''))
				|| (cmd[i] && ft_strchr(cmd[i], '"'))))
			cmd[i] = delete_quote_tok(cmd[i]);
		i++;
	}
}
