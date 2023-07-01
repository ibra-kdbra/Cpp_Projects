#include "../../inc/minishell.h"

static char	*apply_expansion(char *tok, t_env *env);
static char	*replace_var(char *token, int *i, t_env *env);
char		*replace_var_wrongname(char *tok, int *i);
char		*replace_var_exitcode(char *tok, int *i);

void	expand_doll(char **cmd, int *tokens, t_env *env)
{
	int	i;
	int	j;

	i = 0;
	while (cmd[i])
	{
		if (ft_strchr(cmd[i], '$'))
			cmd[i] = apply_expansion(cmd[i], env);
		j = 0;
		while (cmd[i][j])
			if (cmd[i][j] != ' ' || !(cmd[i][j] >= 9 && cmd[i][j] <= 13))
				break ;
		if (!cmd[i][j])
			tokens[i] = EMPTY;
		i++;
	}
}

static char	*apply_expansion_bis(char *tok, int *i, int state, t_env *env);

static char	*apply_expansion(char *tok, t_env *env)
{
	int		i;
	int		state;

	i = 0;
	state = 0;
	while (i != -1 && tok[i])
	{
		chg_qte_state(&state, tok[i]);
		if (tok[i] == '$' && state != 1 && tok[i + 1] == '?')
		{
			tok = replace_var_exitcode(tok, &i);
			if (tok[i] == '$' || tok[i] == '"' || tok[i] == '\'')
				continue ;
			else if (tok[i] == 0)
				break ;
		}
		else if (tok[i] == '$' && state != 1 && (tok[i + 1] == ' '
				|| tok[i + 1] == '\'' || tok[i + 1] == '"' || tok[i + 1] == '$'
				|| !tok[i + 1]))
		;
		else
			tok = apply_expansion_bis(tok, &i, state, env);
		i++;
	}
	return (tok);
}

static char	*apply_expansion_bis(char *tok, int *i, int state, t_env *env)
{
	if ((tok[*i] == '$' && state != 1 && ft_isalpha(tok[*i + 1]))
		|| (tok[*i] == '$' && state != 1 && tok[*i + 1] == '_'))
	{
		tok = replace_var(tok, i, env);
		if (tok[*i] == '$' || tok[*i] == '"' || tok[*i] == '\'')
			*i -= 1;
		else if (tok[*i] == 0)
			*i = -2;
	}
	else if (tok[*i] == '$' && state != 1)
	{
		tok = replace_var_wrongname(tok, i);
		if (tok[*i] == '$' || tok[*i] == '"' || tok[*i] == '\'')
			*i -= 1;
		else if (tok[*i] == 0)
			*i = -2;
	}
	return (tok);
}

static void	ft_free_this(char *tmp, char *tmp2, char *var_name);

static char	*replace_var(char *token, int *i, t_env *env)
{
	t_env	*search;
	char	*tmp;
	char	*tmp2;
	char	*var_name;
	int		end_var_name;

	end_var_name = (*i) + 1;
	while ((token[end_var_name] && ft_isalnum(token[end_var_name]))
		|| (token[end_var_name] && token[end_var_name] == '_'))
		end_var_name++;
	var_name = ft_substr(token, (*i) + 1, end_var_name - (*i) - 1);
	search = ft_envlst_search(env, var_name);
	tmp = ft_substr(token, 0, *i);
	if (search && search->var_value)
		tmp2 = ft_strjoin(tmp, search->var_value);
	else
		tmp2 = ft_strjoin(tmp, "");
	free (tmp);
	tmp = ft_substr(token, end_var_name, 1000);
	free (token);
	token = ft_strjoin(tmp2, tmp);
	*i = ft_strlen(tmp2);
	ft_free_this(tmp, tmp2, var_name);
	return (token);
}

static void	ft_free_this(char *tmp, char *tmp2, char *var_name)
{
	free (tmp);
	free (tmp2);
	free (var_name);
}
