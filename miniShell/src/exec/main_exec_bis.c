#include "../../inc/minishell.h"
#include "../../inc/exec_defs.h"

t_exec	*exec_init(char *raw_cmd, t_env *env)
{
	t_exec	*exec;

	raw_cmd = parse_operator(raw_cmd);
	if (!raw_cmd)
		return (NULL);
	exec = malloc(sizeof(t_exec));
	if (!exec)
		return (NULL);
	exec->cmds_old = args_split(raw_cmd);
	free(raw_cmd);
	if (!exec->cmds_old)
	{
		free(exec);
		return (NULL);
	}
	exec->tok_old = tokenise_line(exec->cmds_old);
	if (!exec->tok_old)
	{
		ft_free_arr((void **)exec->cmds_old);
		free(exec);
		return (NULL);
	}
	expand_doll(exec->cmds_old, exec->tok_old, env);
	remove_quote(exec->cmds_old);
	return (exec);
}

char	***init_cmd_arr(t_exec *exec, char **cmd_split)
{
	char	***cmds;
	int		idx_arg;
	int		idx_cmd;

	cmds = malloc(sizeof(char **) * (exec->cmd_cnt + 1));
	if (!cmds)
	{
		perror("malloc");
		return (NULL);
	}
	cmds[exec->cmd_cnt] = NULL;
	idx_arg = 0;
	idx_cmd = 0;
	while (exec->cmds_old[idx_arg])
	{
		cmds[idx_cmd] = get_nxt_cmd(&cmd_split[idx_arg],
				&exec->tok_old[idx_arg]);
		idx_arg += ft_array_size(cmds[idx_cmd]);
		if (exec->cmds_old[idx_arg])
			idx_arg++;
		idx_cmd++;
	}
	return (cmds);
}

int	**init_tokens_arr(t_exec *exec, int save)
{
	int	**tok_arr;
	int	i;
	int	j;

	tok_arr = malloc(sizeof(int *) * (exec->cmd_cnt + 1));
	if (!tok_arr)
		return (NULL);
	tok_arr[exec->cmd_cnt] = NULL;
	i = 0;
	while (i < exec->cmd_cnt)
	{
		tok_arr[i] = malloc(sizeof(int) * (ft_array_size(exec->cmds[i]) + 1));
		j = 0;
		while (exec->tok_old[save + j] != 0 && exec->tok_old[save + j] != PIPE)
		{
			tok_arr[i][j] = exec->tok_old[save + j];
			j++;
		}
		tok_arr[i][j] = 0;
		save += j + 1;
		i++;
	}
	return (tok_arr);
}

void	get_last_cmd_stat(t_exec *exec, int idx, char *path, char **args)
{
	if (!path && !is_builtin(exec->cmds[idx], exec->toks[idx]) && args)
		g_exit_status = 127;
	if (exec->pipe_fd[idx % 2][1] == -1
		|| exec->pipe_fd[(idx + 1) % 2][0] == -1)
		g_exit_status = 1;
	ft_free_arr((void **)args);
	free(path);
}

void	sec_full_close(int pipe_fd[2][2], int cmd_nb)
{
	sec_close(pipe_fd[(cmd_nb + 1) % 2][0]);
	sec_close(pipe_fd[cmd_nb % 2][0]);
	sec_close(pipe_fd[cmd_nb % 2][1]);
}
