#include "../../inc/minishell.h"

// static void	print_error(int cmd_type);
static bool	check_end_pipe(int *cmd_type);
static bool	check_max_heredoc(int *cmd_type);
static bool	check_file_operator(int *cmd_type);
bool		check_for_double_operators(int *cmd_type);

bool	check_tokens(int *cmd_type)
{
	if (!check_for_double_operators(cmd_type))
	{
		g_exit_status = 2;
		return (false);
	}
	if (check_end_pipe(cmd_type))
	{
		g_exit_status = 2;
		return (false);
	}
	if (!check_file_operator(cmd_type))
	{
		g_exit_status = 2;
		ft_dprintf(2, "minishell: syntax error: near unexpected token\n");
		return (false);
	}
	if (!check_max_heredoc(cmd_type))
		return (false);
	return (true);
}

static bool	check_end_pipe(int *cmd_type)
{
	int	i;

	i = 0;
	while (cmd_type[i])
		i++;
	if (cmd_type[i - 1] && cmd_type[i - 1] == PIPE)
	{
		ft_dprintf(2, "minishell: parse error near `|'\n");
		return (true);
	}
	return (false);
}

static bool	check_max_heredoc(int *cmd_type)
{
	int	i;
	int	nb_heredoc;

	i = 0;
	nb_heredoc = 0;
	while (cmd_type[i])
	{
		if (cmd_type[i] == HEREDOC_SIG)
			nb_heredoc += 1;
		i++;
	}
	if (nb_heredoc > 16)
	{
		g_exit_status = 2;
		ft_dprintf(2, "minishell: maximum here-document exceeded\n");
		return (false);
	}
	return (true);
}

static bool	check_file_operator(int *cmd_type)
{
	int	i;

	i = 0;
	while (cmd_type[i])
	{
		if (cmd_type[i] == R_IN_SIG && cmd_type[i + 1] != R_IN_FILE)
			return (false);
		if (cmd_type[i] == R_OUT_SIG && cmd_type[i + 1] != R_OUT_FILE)
			return (false);
		if (cmd_type[i] == R_APP_SIG && cmd_type[i + 1] != R_APP_FILE)
			return (false);
		if (cmd_type[i] == HEREDOC_SIG && cmd_type[i + 1] != HEREDOC_LIM)
			return (false);
		i++;
	}
	return (true);
}
