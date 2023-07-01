#include "../inc/minishell.h"

void	default_sigint(int sig)
{
	(void)sig;
	g_exit_status = 130;
	ft_dprintf(1, "\n");
	rl_on_new_line();
	rl_replace_line("", 1);
	rl_redisplay();
}

void	hdsig(int sig)
{
	(void)sig;
	close(0);
}

void	sig_parent(int sig)
{
	if (sig == SIGINT)
		ft_dprintf(1, "\n");
	else if (sig == SIGQUIT)
		ft_dprintf(2, "Quit (code dumped)\n");
}

void	handle_sig(int state)
{
	if (state == SIG_DEFAULT)
	{
		signal(SIGINT, default_sigint);
		signal(SIGQUIT, SIG_IGN);
	}
	else if (state == SIG_HEREDOC)
	{
		signal(SIGINT, &hdsig);
		signal(SIGQUIT, SIG_IGN);
	}
	else if (state == SIG_CHILD)
	{
		signal(SIGQUIT, SIG_DFL);
		signal(SIGINT, SIG_DFL);
	}
	else if (state == SIG_PARENT)
	{
		signal(SIGINT, &sig_parent);
		signal(SIGQUIT, &sig_parent);
	}
}
