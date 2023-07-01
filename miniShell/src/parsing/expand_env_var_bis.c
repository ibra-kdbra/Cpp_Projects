#include "../../inc/minishell.h"

char	*replace_var_wrongname(char *tok, int *i)
{
	char	*tmp;
	char	*tmp2;

	tmp = ft_strdup(tok);
	tmp[*i] = 0;
	tmp2 = ft_substr(tok, (*i) + 2, 1000);
	free (tok);
	tok = ft_strjoin(tmp, tmp2);
	*i = ft_strlen(tmp);
	free (tmp);
	free (tmp2);
	return (tok);
}

char	*replace_var_exitcode(char *tok, int *i)
{
	char	*tmp;
	char	*tmp2;
	char	*itoa_exitcode;

	itoa_exitcode = ft_itoa(g_exit_status);
	tmp = ft_substr(tok, 0, *i);
	tmp2 = ft_strjoin(tmp, itoa_exitcode);
	free (itoa_exitcode);
	free (tmp);
	tmp = ft_substr(tok, (*i) + 2, 1000);
	free (tok);
	tok = ft_strjoin(tmp2, tmp);
	*i = ft_strlen(tmp2);
	free (tmp2);
	free (tmp);
	return (tok);
}
