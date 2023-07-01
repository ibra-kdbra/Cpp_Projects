#include "../../inc/minishell.h"
#include "../../inc/exec_defs.h"
#include <stdlib.h>

static char	**get_split_path(t_env *env);
static char	*get_path(char *cmd, char **split_paths, int idx, int acs);

char	*get_cmd_path(char **cmd_arg, int *tokens, t_env *env)
{
	char	*cmd;
	char	**paths;
	int		idx;
	int		acs;

	idx = 0;
	while (cmd_arg[idx] && tokens[idx] != CMD_NAME && tokens[idx] != CMD_ARG)
		idx++;
	if (!cmd_arg[idx] || !cmd_arg[idx][0])
		return (NULL);
	paths = get_split_path(env);
	acs = access(cmd_arg[idx], X_OK);
	cmd = get_path(cmd_arg[idx], paths, 0, acs);
	ft_free_arr((void **)paths);
	return (cmd);
}

static char	*get_path(char *cmd, char **split_paths, int idx, int acs)
{
	char	*path_of_cmd;

	if (!acs)
		return (ft_strdup(cmd));
	if (!split_paths)
	{
		perror(cmd);
		return (NULL);
	}
	cmd = ft_strjoin("/", cmd);
	while (split_paths && split_paths[idx] && acs == -1)
	{
		path_of_cmd = ft_strjoin(split_paths[idx++], cmd);
		acs = access(path_of_cmd, X_OK);
		if (acs == -1)
			free(path_of_cmd);
	}
	free(cmd);
	if (acs == -1)
		return (NULL);
	return (path_of_cmd);
}

static char	**get_split_path(t_env *env)
{
	char	**split_path;
	t_env	*link;

	link = env;
	while (link && ft_strcmp(link->var_name, "PATH"))
		link = link->next;
	split_path = NULL;
	if (link)
		split_path = ft_split(link->var_value, ':');
	return (split_path);
}

static int	get_arg_nb(int *tokens);

char	**get_cmd_args(char **cmd_arg, int *tokens, t_env *env, char *cmd)
{
	char	**args;
	int		nb_arg;
	int		idx;

	args = get_split_path(env);
	if (!args && env && !cmd)
		return (NULL);
	ft_free_arr((void **)args);
	nb_arg = get_arg_nb(tokens);
	if (!nb_arg)
		return (NULL);
	args = malloc(sizeof(char *) * (nb_arg + 1));
	if (!args)
		return (NULL);
	args[nb_arg] = NULL;
	idx = -1;
	nb_arg = 0;
	while (cmd_arg[++idx])
		if (tokens[idx] == CMD_NAME || tokens[idx] == CMD_ARG)
			args[nb_arg++] = ft_strdup(cmd_arg[idx]);
	return (args);
}

static int	get_arg_nb(int *tokens)
{
	int	idx;
	int	cnt;

	cnt = 0;
	idx = 0;
	while (tokens[idx])
	{
		if (tokens[idx] == CMD_ARG || tokens[idx] == CMD_NAME)
			cnt++;
		idx++;
	}
	return (cnt);
}
