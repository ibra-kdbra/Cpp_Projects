#include "../inc/minishell.h"
#include "../inc/exec_defs.h"

int	g_exit_status;

static void	check_args(int argc, char *argv[])
{
	(void) argv;
	if (argc > 1)
	{
		ft_dprintf(2, "minishell: too many args\n");
		exit (127);
	}
}

int	main(int argc, char *argv[], char *envp[])
{
	char	*cmd_line;
	t_env	*env;

	check_args(argc, argv);
	g_exit_status = 0;
	env = get_env_variables(envp);
	handle_sig(SIG_DEFAULT);
	get_prompt(env);
	cmd_line = readline("$ ");
	while (cmd_line)
	{
		if (*cmd_line)
			add_history(cmd_line);
		if (parse_line(cmd_line))
			main_exec(cmd_line, &env);
		get_prompt(env);
		cmd_line = readline("$ ");
	}
	ft_envlst_clear(&env);
	rl_clear_history();
	ft_dprintf(1, "exit\n");
	return (g_exit_status);
}
