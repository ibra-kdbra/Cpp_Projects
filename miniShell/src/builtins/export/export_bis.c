#include "../../../inc/minishell.h"

static char	*get_var_name(const char *args);

void	export_concatenate(char *args, t_env **env, char *var_old_value)
{
	char	*var_name;
	t_env	*search;

	var_name = get_var_name(args);
	search = ft_envlst_search(*env, var_name);
	free (var_name);
	if (search && search->var_value)
	{
		var_old_value = ft_strdup(search->var_value);
		free (search->var_value);
		search->var_value = ft_strjoin(var_old_value, ft_strchr(args, '=') + 1);
		free (var_old_value);
	}
	else if (search)
	{
		ft_envlst_clearone(env, search);
		ft_envlstadd_back(env, ft_envlst_new(args));
	}
	else
		ft_envlstadd_back(env, ft_envlst_new(args));
}

void	export_assign(char *args, t_env **env)
{
	char	*var_name;
	t_env	*search;

	var_name = get_var_name(args);
	search = ft_envlst_search(*env, var_name);
	free (var_name);
	if (search)
	{
		free (search->var_value);
		search->var_value = ft_strdup(ft_strchr(args, '=') + 1);
	}
	else
		ft_envlstadd_back(env, ft_envlst_new(args));
}

void	export_create_var(char *args, t_env **env)
{
	char	*var_name;
	t_env	*search;

	var_name = get_var_name(args);
	search = ft_envlst_search(*env, var_name);
	free (var_name);
	if (!search)
		ft_envlstadd_back(env, (ft_envlst_new(args)));
}

static char	*get_var_name(const char *args)
{
	int		i;
	char	*var_name;

	i = 0;
	while (args[i])
	{
		if (args[i] == '+' || args[i] == '=')
			break ;
		i++;
	}
	var_name = malloc(sizeof(char) * i + 1);
	if (!var_name)
		return (NULL);
	ft_strlcpy(var_name, args, i + 1);
	return (var_name);
}
