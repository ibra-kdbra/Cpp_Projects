#include "../../inc/minishell.h"

static char	*add_space_before_operator(char *str, int i);
static char	*add_space_after_operator(char *str, int i);
static char	*parse_after_operator(char *cmd);
static char	*check_after_operator(char *new_cmd, int i, int *operator_length);

char	*parse_operator(char *cmd)
{
	int		i;
	int		state;
	char	*new_cmd;

	i = 0;
	state = 0;
	new_cmd = ft_strdup(cmd);
	free (cmd);
	while (new_cmd[i])
	{
		chg_qte_state(&state, new_cmd[i]);
		if (state == 0 && (is_redirection(new_cmd[i]) || is_pipe(new_cmd[i])))
		{
			if (i > 1 && ft_strlen(new_cmd) > 1
				&& !is_space(new_cmd[i - 1]) && !is_redirection(new_cmd[i - 1]))
				new_cmd = add_space_before_operator(new_cmd, i);
		}
		i++;
	}
	new_cmd = parse_after_operator(new_cmd);
	return (new_cmd);
}

static char	*parse_after_operator(char *cmd)
{
	char	*new_cmd;
	int		i;
	int		state;
	int		operator_length;

	i = -1;
	operator_length = 0;
	state = 0;
	new_cmd = ft_strdup(cmd);
	free (cmd);
	while (new_cmd[++i])
	{	
		chg_qte_state(&state, new_cmd[i]);
		if (state == 0 && (is_redirection(new_cmd[i]) || is_pipe(new_cmd[i])))
		{
			if (is_redirection(new_cmd[i]))
				operator_length += 1;
			new_cmd = check_after_operator(new_cmd, i, &operator_length);
		}
	}
	return (new_cmd);
}

static char	*check_after_operator(char *new_cmd, int i, int *operator_length)
{
	if ((new_cmd[i + 1] && !is_space(new_cmd[i + 1])
			&& !is_redirection(new_cmd[i + 1])) || *operator_length == 2)
	{
		new_cmd = add_space_after_operator(new_cmd, i);
		*operator_length = 0;
	}
	else
		*operator_length = 0;
	return (new_cmd);
}

static char	*add_space_before_operator(char *str, int i)
{
	char	*new_str;
	char	*tmp;
	char	*tmp2;

	tmp = ft_substr(str, 0, i);
	tmp2 = ft_strjoin(tmp, " ");
	free (tmp);
	tmp = ft_substr(str, i, 1000);
	new_str = ft_strjoin(tmp2, tmp);
	free (tmp);
	free (tmp2);
	free (str);
	return (new_str);
}

static char	*add_space_after_operator(char *str, int i)
{
	char	*new_str;
	char	*tmp;
	char	*tmp2;

	tmp = ft_substr(str, 0, i + 1);
	tmp2 = ft_strjoin(tmp, " ");
	free (tmp);
	tmp = ft_substr(str, i + 1, 1000);
	new_str = ft_strjoin(tmp2, tmp);
	free (tmp);
	free (tmp2);
	free (str);
	return (new_str);
}
