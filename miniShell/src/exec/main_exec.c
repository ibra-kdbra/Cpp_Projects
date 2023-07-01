#include "../../inc/minishell.h"
#include "../../inc/exec_defs.h"

t_exec			*exec_init(char *raw_cmd, t_env *env);
char			***init_cmd_arr(t_exec *exec, char **cmd_split);
int				**init_tokens_arr(t_exec *exec, int save);
void			get_last_cmd_stat(t_exec *exec,
					int idx, char *path, char **args);
static void		launch_exec(t_exec *exec);
static void		cmd_exec(char *path, char **args, int cmd_nb, t_exec *exec);
static void		cmd_loop(t_exec *exec, int cmd,
					char *cmd_path, char **cmd_args);
static void		wait_children(int *cpid, int cmds_cnt);

void	main_exec(char *raw_cmd, t_env **env)
{
	t_exec	*exec;
	int		i;

	exec = exec_init(raw_cmd, *env);
	if (!exec || !exec->tok_old)
		return ;
	exec->env = env;
	exec->cmd_cnt = cmd_counter(exec->cmds_old, exec->tok_old);
	exec->cmds = init_cmd_arr(exec, exec->cmds_old);
	exec->toks = init_tokens_arr(exec, 0);
	exec->cpid = malloc(sizeof(int) * (exec->cmd_cnt + 1));
	if (heredoc_init(exec->heredoc_fd, exec->cmds, exec->toks))
		g_exit_status = 130;
	else if (exec->cpid)
	{
		i = 0;
		while (i < exec->cmd_cnt)
			exec->cpid[i++] = -1;
		exec->cpid[i] = 0;
		launch_exec(exec);
	}
	free_exec(exec, NULL, NULL);
}

static void	launch_exec(t_exec *exec)
{
	exec->pipe_fd[0][0] = -1;
	exec->pipe_fd[0][1] = -1;
	exec->pipe_fd[1][0] = -1;
	exec->pipe_fd[1][1] = -1;
	if (isbuiltin_env(exec->cmds[0], exec->toks[0]) && exec->cmd_cnt == 1)
	{
		redirection_hdl(exec, 0);
		start_builtin_env(exec->cmds[0], exec->toks[0], exec);
		sec_close(exec->pipe_fd[0][1]);
		sec_close(exec->pipe_fd[1][0]);
		return ;
	}
	else
		cmd_loop(exec, 0, NULL, NULL);
	handle_sig(SIG_DEFAULT);
}

static void	cmd_loop(t_exec *exec, int cmd, char *cmd_path, char **cmd_args)
{
	while (exec->cmds[cmd])
	{
		redirection_hdl(exec, cmd);
		cmd_path = NULL;
		cmd_args = NULL;
		if (is_builtin(exec->cmds[cmd], exec->toks[cmd]))
			init_builtin_exec(exec->cmds[cmd], exec->toks[cmd], exec, cmd);
		else
		{
			cmd_path = get_cmd_path(exec->cmds[cmd],
					exec->toks[cmd], *exec->env);
			cmd_args = get_cmd_args(exec->cmds[cmd],
					exec->toks[cmd], *exec->env, cmd_path);
			cmd_exec(cmd_path, cmd_args, cmd, exec);
			if (cmd < exec->cmd_cnt - 1)
			{
				free(cmd_path);
				ft_free_arr((void **)cmd_args);
			}
		}
		sec_close(exec->pipe_fd[(++cmd % 2)][0]);
		sec_close(exec->pipe_fd[(cmd + 1) % 2][1]);
	}
	wait_children(exec->cpid, exec->cmd_cnt);
	get_last_cmd_stat(exec, cmd - 1, cmd_path, cmd_args);
}

static void	wait_children(int *cpid, int cmds_cnt)
{
	int	pid;
	int	status;

	pid = 0;
	while (pid < cmds_cnt)
	{
		if (cpid[pid] != -1)
		{
			waitpid(cpid[pid], &status, 0);
			if (WIFEXITED(status))
				g_exit_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
			{
				if (WTERMSIG(status) == 2)
					g_exit_status = 130;
				else if (WTERMSIG(status) == 3)
					g_exit_status = 131;
				else if (WTERMSIG(status) == 11)
					g_exit_status = 139;
			}
		}
		pid++;
	}
}

static void	cmd_exec(char *path, char **args, int cmd_nb, t_exec *exec)
{
	char	**envp;

	if (exec->pipe_fd[(cmd_nb + 1) % 2][0] == -1
		|| exec->pipe_fd[cmd_nb % 2][1] == -1 || !args || check_dir(args[0]))
		return ;
	if (path == NULL && args[0])
	{
		ft_dprintf(2, "minishell: %s: command not found\n", args[0]);
		return ;
	}
	handle_sig(SIG_PARENT);
	exec->cpid[cmd_nb] = fork();
	if (exec->cpid[cmd_nb] == 0)
	{
		handle_sig(SIG_CHILD);
		dup2(exec->pipe_fd[(cmd_nb + 1) % 2][0], 0);
		dup2(exec->pipe_fd[cmd_nb % 2][1], 1);
		sec_full_close(exec->pipe_fd, cmd_nb);
		envp = transform_envp(*exec->env);
		execve(path, args, envp);
		ft_envlst_clear((exec)->env);
		ft_free_arr((void **)envp);
		free_exec(exec, path, args);
		exit(1);
	}
}
