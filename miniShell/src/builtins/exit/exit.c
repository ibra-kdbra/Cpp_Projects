#include "../../../inc/minishell.h"
#include "../../../inc/exec_defs.h"

static void	exit_2args(char **args, t_exec *exec);
static void	exit_moreargs(char **args, t_exec *exec);
static bool	is_first_arg_digit(char *str);

void	ft_exit(char **args, t_exec *exec)
{
	if (ft_array_size(args) == 1)
	{
		ft_dprintf(1, "exit\n");
		ft_envlst_clear(exec->env);
		free_exec(exec, NULL, args);
		exit (g_exit_status);
	}
	else if (ft_array_size(args) == 2)
		exit_2args(args, exec);
	else
		exit_moreargs(args, exec);
}

static void	exit_2args(char **args, t_exec *exec)
{
	unsigned char	stat;

	if (is_first_arg_digit(args[1]))
	{
		ft_dprintf(1, "exit\n");
		ft_envlst_clear(exec->env);
		stat = ft_atoll(args[1]);
		free_exec(exec, NULL, args);
		exit (stat);
	}
	else
	{
		ft_dprintf(1, "exit\n");
		ft_dprintf(2,
			"minishell: exit: %s: numeric argument required\n", args[1]);
		ft_envlst_clear(exec->env);
		free_exec(exec, NULL, args);
		exit (2);
	}
}

static void	exit_moreargs(char **args, t_exec *exec)
{
	if (is_first_arg_digit(args[1]))
	{
		ft_dprintf(1, "exit\n");
		ft_dprintf(2, "minishell: exit: too many arguments\n");
		g_exit_status = 1;
	}
	else
	{
		ft_dprintf(1, "exit\n");
		ft_dprintf(2,
			"minishell: exit: %s: numeric argument required\n", args[1]);
		ft_envlst_clear(exec->env);
		free_exec(exec, NULL, args);
		exit (2);
	}
}

static bool	is_first_arg_digit(char *str)
{
	int	i;
	int	error;

	i = 0;
	ft_erratoll(str, &error);
	if (error != 0)
		return (false);
	if (str[i] == '-' || str[i] == '+')
		i++;
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (false);
		i++;
	}
	return (true);
}
