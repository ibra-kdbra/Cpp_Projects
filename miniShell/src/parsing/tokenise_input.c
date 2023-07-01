#include "../../inc/minishell.h"

static bool	is_in_charset(char c, char *charset);
static void	define_token_operator(char *cmd, int *cmd_type);
static void	define_token_type(char **cmd, int **cmd_type,
				int i, bool *cmd_in_pipe);

int	*tokenise_line(char **cmd)
{
	bool	cmd_in_pipe;
	int		*cmd_type;
	int		i;

	cmd_type = ft_calloc(sizeof(int), ft_array_size(cmd) + 1);
	if (!cmd_type)
		return (NULL);
	i = 0;
	cmd_in_pipe = false;
	while (cmd[i])
	{
		define_token_type(cmd, &cmd_type, i, &cmd_in_pipe);
		i++;
	}
	if (check_tokens(cmd_type))
		return (cmd_type);
	g_exit_status = 2;
	free(cmd_type);
	return (NULL);
}

static void	define_token_type(char **cmd, int **cmd_type,
				int i, bool *cmd_in_pipe)
{	
	if (is_in_charset(cmd[i][0], "<>|"))
	{
		define_token_operator(cmd[i], &(*cmd_type)[i]);
		if ((*cmd_type)[i] == PIPE)
			*cmd_in_pipe = false;
	}
	else if (i > 0 && (*cmd_type)[i - 1] == HEREDOC_SIG)
		(*cmd_type)[i] = HEREDOC_LIM;
	else if (i > 0 && (*cmd_type)[i - 1] == R_IN_SIG)
		(*cmd_type)[i] = R_IN_FILE;
	else if (i > 0 && (*cmd_type)[i - 1] == R_OUT_SIG)
		(*cmd_type)[i] = R_OUT_FILE;
	else if (i > 0 && (*cmd_type)[i - 1] == R_APP_SIG)
		(*cmd_type)[i] = R_APP_FILE;
	else if ((i > 0 && (*cmd_type)[i - 1] == CMD_NAME)
			|| (i > 0 && (*cmd_type)[i - 1] == CMD_ARG) || *cmd_in_pipe == true)
		(*cmd_type)[i] = CMD_ARG;
	else
	{
		(*cmd_type)[i] = CMD_NAME;
		*cmd_in_pipe = true;
	}
}

static void	define_token_operator(char *cmd, int *cmd_type)
{
	if (cmd[0] == '|')
		*cmd_type = PIPE;
	else if (ft_strlen(cmd) > 1 && cmd[0] == '<')
		*cmd_type = HEREDOC_SIG;
	else if (ft_strlen(cmd) > 1 && cmd[0] == '>')
		*cmd_type = R_APP_SIG;
	else if (cmd[0] == '<')
		*cmd_type = R_IN_SIG;
	else if (cmd[0] == '>')
		*cmd_type = R_OUT_SIG;
}

static bool	is_in_charset(char c, char *charset)
{
	int	i;

	i = 0;
	while (charset[i])
	{
		if (charset[i] == c)
			return (true);
		i++;
	}
	return (false);
}
