#include "../../../inc/minishell.h"

void		print_env_var_ascii(t_env *env);
static bool	parse_env_var_name(const char *args, int *exit_code);
static void	execute_export(char *args, t_env **env);

void	ft_export(t_env **env, char **args)
{
	int		i;
	int		exit_code;

	if (ft_array_size(args) == 1)
	{
		print_env_var_ascii(*env);
		return ;
	}
	i = 1;
	exit_code = 0;
	while (args[i])
	{
		if (parse_env_var_name(args[i], &exit_code))
			execute_export(args[i], env);
		else
			exit_code = 1;
		i++;
	}
	g_exit_status = 0;
	if (exit_code == 1)
		g_exit_status = 1;
	return ;
}

static bool	parse_env_var_name(const char *args, int *exit_code)
{
	int	i;

	if (!ft_isalpha(args[0]) && args[0] != '_')
	{
		*exit_code = 1;
		ft_dprintf(2, "minishell: export: `%s': not a valid identifier\n", args);
		return (false);
	}
	i = 0;
	while (args[i])
	{
		if (args[i] == '=' || (args[i] == '+' && args[i + 1] == '='))
			break ;
		if (!ft_isalnum(args[i]) && args[i] != '_')
		{
			*exit_code = 1;
			ft_dprintf(2,
				"minishell: export: `%s': not a valid identifier\n", args);
			return (false);
		}
		i++;
	}
	return (true);
}

void		export_concatenate(char *args, t_env **env, char *var_old_value);
void		export_assign(char *args, t_env **env);
void		export_create_var(char *args, t_env **env);

static void	execute_export(char *args, t_env **env)
{
	if (ft_strchr(args, '=')
		&& (*(ft_strchr(args, '=') - 1) == '+'))
		export_concatenate(args, env, NULL);
	else if (ft_strchr(args, '='))
		export_assign(args, env);
	else
		export_create_var(args, env);
}
