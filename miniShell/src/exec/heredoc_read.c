#include "../../inc/minishell.h"
#include "../../inc/exec_defs.h"
#include <readline/readline.h>
#include <signal.h>
#include <unistd.h>

static void	heredoc_open(int hd_arr[16][2], char ***cmds, int **toks);
static void	read_stdin(char *limit, int wr_fd);
static int	read_heredoc(char *limit);
static void	init_hd_arr(int hd_arr[16][2]);
void		close_heredocs(int hd_arr[16][2]);

int	heredoc_init(int hd_arr[16][2], char ***cmds, int **toks)
{
	int	stdinbackup;

	stdinbackup = dup(0);
	heredoc_open(hd_arr, cmds, toks);
	if (!stdin_state())
	{
		dup2(stdinbackup, 0);
		close(stdinbackup);
		close_heredocs(hd_arr);
		return (1);
	}
	close(stdinbackup);
	return (0);
}

static void	heredoc_open(int hd_arr[16][2], char ***cmds, int **toks)
{
	int	cmd_nb;
	int	idx;
	int	arr_idx;

	cmd_nb = 0;
	arr_idx = 0;
	init_hd_arr(hd_arr);
	while (cmds[cmd_nb] && stdin_state())
	{
		idx = 0;
		while (cmds[cmd_nb][idx] && stdin_state())
		{
			if (toks[cmd_nb][idx] == HEREDOC_LIM)
			{
				if (hd_arr[arr_idx][0] == cmd_nb)
					sec_close(hd_arr[arr_idx][1]);
				hd_arr[arr_idx][1] = read_heredoc(cmds[cmd_nb][idx]);
				hd_arr[arr_idx][0] = cmd_nb;
			}
			idx++;
		}
		if (hd_arr[arr_idx][0] == cmd_nb)
			arr_idx++;
		cmd_nb++;
	}
}

static void	init_hd_arr(int hd_arr[16][2])
{
	int	i;

	i = 0;
	while (i < 16)
	{
		hd_arr[i][0] = -1;
		hd_arr[i][1] = -1;
		i++;
	}
}

static int	read_heredoc(char *limit)
{
	int		fds[2];

	if (pipe(fds) == -1)
	{
		perror("pipe");
		return (-1);
	}
	read_stdin(limit, fds[1]);
	close(fds[1]);
	return (fds[0]);
}

static void	read_stdin(char *limit, int wr_fd)
{
	char	*buf;

	handle_sig(SIG_HEREDOC);
	buf = readline("heredoc> ");
	while (buf && ft_strcmp(buf, limit))
	{
		ft_dprintf(wr_fd, "%s\n", buf);
		free(buf);
		buf = readline("heredoc> ");
	}
	if (!stdin_state())
		ft_dprintf(1, "\n");
	else if (!buf)
		ft_dprintf(2, HD_EOF, limit);
	free(buf);
	handle_sig(SIG_DEFAULT);
	return ;
}
