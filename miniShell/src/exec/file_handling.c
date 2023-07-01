#include "../../inc/minishell.h"
#include "../../inc/exec_defs.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

static void	file_finder(t_exec *exec, int cmd_nb, int pipe_fd[2][2], int i);
static int	open_outfile(char *filename, int mode);
static int	open_infile(char *filename, int mode);
static void	close_unused_hd(int heredoc_fd[16][2], int cmd_nb);

void	redirection_hdl(t_exec *exec, int cmd_nb)
{
	if (exec->cmd_cnt == 1 && cmd_nb == 0)
	{
		exec->pipe_fd[1][0] = dup(0);
		exec->pipe_fd[0][1] = dup(1);
		file_finder(exec, cmd_nb, exec->pipe_fd, -1);
		return ;
	}
	if (cmd_nb == 0)
	{
		pipe(exec->pipe_fd[0]);
		exec->pipe_fd[1][0] = dup(0);
		file_finder(exec, cmd_nb, exec->pipe_fd, -1);
		return ;
	}
	if (cmd_nb == exec->cmd_cnt - 1)
	{
		exec->pipe_fd[cmd_nb % 2][1] = dup(1);
		file_finder(exec, cmd_nb, exec->pipe_fd, -1);
		return ;
	}
	pipe(exec->pipe_fd[cmd_nb % 2]);
	file_finder(exec, cmd_nb, exec->pipe_fd, -1);
}

static void	file_finder(t_exec *exec, int cmd_nb, int pipe_fd[2][2], int i)
{
	int	fds[2];

	fds[0] = pipe_fd[(cmd_nb + 1) % 2][0];
	fds[1] = pipe_fd[cmd_nb % 2][1];
	while (exec->cmds[cmd_nb][++i] && fds[0] != -1 && fds[1] != -1)
	{
		if (exec->toks[cmd_nb][i] == R_OUT_FILE
			|| exec->toks[cmd_nb][i] == R_APP_FILE)
		{
			sec_close(fds[1]);
			fds[1] = open_outfile(exec->cmds[cmd_nb][i], exec->toks[cmd_nb][i]);
		}
		else if (exec->toks[cmd_nb][i] == R_IN_FILE)
		{
			sec_close(fds[0]);
			fds[0] = open_infile(exec->cmds[cmd_nb][i], exec->toks[cmd_nb][i]);
		}
	}
	if (fds[0] != -1 && fds[1] != -1)
		fds[0] = check_heredoc_use(fds[0], exec, cmd_nb);
	else
		close_unused_hd(exec->heredoc_fd, cmd_nb);
	pipe_fd[(cmd_nb + 1) % 2][0] = fds[0];
	pipe_fd[cmd_nb % 2][1] = fds[1];
}

static int	open_outfile(char *filename, int mode)
{
	int	fd;
	int	open_mode;

	open_mode = O_TRUNC;
	if (mode == R_APP_FILE)
		open_mode = O_APPEND;
	fd = open(filename, O_WRONLY | O_CREAT | open_mode, 0644);
	if (fd == -1)
	{
		ft_dprintf(2, "minishell: ");
		perror(filename);
	}
	return (fd);
}

static int	open_infile(char *filename, int mode)
{
	int	fd;

	if (mode == HEREDOC_LIM)
		return (0);
	fd = open(filename, O_RDONLY);
	if (fd == -1)
	{
		ft_dprintf(2, "minishell: ");
		perror(filename);
	}
	return (fd);
}

static void	close_unused_hd(int heredoc_fd[16][2], int cmd_nb)
{
	int	i;

	i = 0;
	while (i < 16)
	{
		if (heredoc_fd[i][0] == cmd_nb)
			close(heredoc_fd[i][1]);
		i++;
	}
}
