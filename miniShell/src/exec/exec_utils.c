#include "../../inc/minishell.h"

int	cmd_counter(char **args, int *tokens)
{
	int	i;
	int	cnt;

	i = 0;
	cnt = 0;
	while (args[i] && tokens[i])
	{
		if (tokens[i] == PIPE)
			cnt++;
		i++;
	}
	return (cnt + 1);
}

char	**get_nxt_cmd(char **cmd_split, int *tokens)
{
	char	**nxt_cmd;
	int		len_arr;
	int		idx;

	len_arr = 0;
	while (cmd_split[len_arr] && tokens[len_arr] != PIPE)
		len_arr++;
	nxt_cmd = malloc(sizeof(char *) * (len_arr + 1));
	if (!nxt_cmd)
	{
		perror("malloc");
		return (NULL);
	}
	nxt_cmd[len_arr] = NULL;
	idx = 0;
	while (idx < len_arr)
	{
		nxt_cmd[idx] = cmd_split[idx];
		idx++;
	}
	return (nxt_cmd);
}

void	sec_close(int fd)
{
	if (fd != -1)
		close(fd);
}

char	**transform_envp(t_env *env)
{
	int		i;
	char	*tmp;
	char	**env_array;

	env_array = malloc(sizeof(char *) * (ft_envlst_size(env) + 1));
	i = 0;
	while (env != NULL)
	{
		if (env->var_value)
		{
			tmp = ft_strjoin(env->var_name, "=");
			env_array[i] = ft_strjoin(tmp, env->var_value);
			free (tmp);
		}
		else
			env_array[i] = ft_strdup(env->var_name);
		env = env->next;
		i++;
	}
	env_array[i] = NULL;
	return (env_array);
}

void	free_exec(t_exec *exec, char *path, char **args)
{
	free(path);
	ft_free_arr((void **)args);
	ft_free_arr((void **)exec->cmds);
	ft_free_arr((void **)exec->toks);
	free(exec->tok_old);
	ft_free_arr((void **)exec->cmds_old);
	free(exec->cpid);
	free(exec);
}
