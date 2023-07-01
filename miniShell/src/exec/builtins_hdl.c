#include "../../inc/minishell.h"
#include "../../inc/def_struct.h"
#include "../libft/libft.h"
#include "../../inc/exec_defs.h"
#include <stdbool.h>

bool	isbuiltin_env(char **cmd, int *toks)
{
	int	i;

	i = 0;
	while (cmd && cmd[i] && toks && toks[i] != CMD_NAME)
		i++;
	if (!cmd[i])
		return (false);
	if (!ft_strcmp(cmd[i], "cd")
		|| !ft_strcmp(cmd[i], "export")
		|| !ft_strcmp(cmd[i], "unset")
		|| !ft_strcmp(cmd[i], "exit"))
		return (true);
	return (false);
}

void	start_builtin_env(char **cmd, int *toks, t_exec *exec)
{
	char	**args;
	int		i;

	i = 0;
	if (!cmd || !toks)
		return ;
	while (cmd[i] && toks[i] != CMD_NAME)
		i++;
	if (!cmd[i])
		return ;
	args = get_cmd_args(cmd, toks, NULL, NULL);
	if (!ft_strcmp(cmd[i], "cd"))
		ft_cd(args, *exec->env);
	else if (!ft_strcmp(cmd[i], "export"))
		ft_export(exec->env, args);
	else if (!ft_strcmp(cmd[i], "unset"))
		ft_unset(exec->env, args);
	else if (!ft_strcmp(cmd[i], "exit"))
	{
		close(exec->pipe_fd[0][1]);
		close(exec->pipe_fd[1][0]);
		ft_exit(args, exec);
	}
	ft_free_arr((void **)args);
}

static void	start_builtin(char **cmds, int *toks, t_exec *exec, int cmd_nb)
{
	char	**args;

	args = get_cmd_args(cmds, toks, NULL, NULL);
	if (!ft_strcmp(args[0], "echo"))
		ft_echo(args);
	else if (!ft_strcmp(args[0], "env"))
		ft_env(*exec->env, args);
	else if (!ft_strcmp(args[0], "pwd"))
		ft_pwd();
	else if (!ft_strcmp(args[0], "cd"))
		ft_cd(args, *exec->env);
	else if (!ft_strcmp(args[0], "export"))
		ft_export(exec->env, args);
	else if (!ft_strcmp(args[0], "unset"))
		ft_unset(exec->env, args);
	else if (!ft_strcmp(args[0], "exit"))
	{
		sec_close(exec->pipe_fd[cmd_nb % 2][0]);
		ft_exit(args, exec);
	}
	ft_free_arr((void **)args);
}

void	init_builtin_exec(char **cmds, int *toks, t_exec *exec, int cmd)
{
	if (exec->pipe_fd[(cmd + 1) % 2][0] == -1
		|| exec->pipe_fd[cmd % 2][1] == -1)
		return ;
	exec->cpid[cmd] = fork();
	if (exec->cpid[cmd] == 0)
	{
		dup2(exec->pipe_fd[(cmd + 1) % 2][0], 0);
		dup2(exec->pipe_fd[cmd % 2][1], 1);
		sec_close(exec->pipe_fd[(cmd + 1) % 2][0]);
		sec_close(exec->pipe_fd[cmd % 2][1]);
		start_builtin(cmds, toks, exec, cmd);
		sec_close(exec->pipe_fd[cmd % 2][0]);
		close_heredocs(exec->heredoc_fd);
		ft_envlst_clear(exec->env);
		free_exec(exec, NULL, NULL);
		exit(g_exit_status);
	}
}

bool	is_builtin(char **cmds, int *toks)
{
	int	i;

	i = 0;
	while (cmds && cmds[i] && toks && toks[i] != CMD_NAME && toks[i] != CMD_ARG)
		i++;
	if (!cmds[i])
		return (false);
	if (!ft_strcmp(cmds[i], "echo") || !ft_strcmp(cmds[i], "env")
		|| !ft_strcmp(cmds[i], "pwd") || !ft_strcmp(cmds[i], "cd")
		|| !ft_strcmp(cmds[i], "export") || !ft_strcmp(cmds[i], "unset")
		|| !ft_strcmp(cmds[i], "exit"))
		return (true);
	return (false);
}
