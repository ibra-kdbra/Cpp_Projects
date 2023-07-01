#include "../../../inc/minishell.h"

void	ft_free_env_arrays(char **env_array_name, char **env_array_value)
{
	int	i;
	int	array_size;

	i = 0;
	array_size = ft_array_size(env_array_name);
	while (i <= array_size)
	{
		free (env_array_name[i]);
		free (env_array_value[i]);
		i++;
	}
	free (env_array_name);
	free (env_array_value);
}

void	print_sorted(char **array_name, char **array_value)
{
	int	i;

	i = 0;
	while (array_name[i])
	{
		if (!(array_name[i][0] == '_' && ft_strlen(array_name[i]) == 1))
		{
			if (array_value[i])
				ft_dprintf(1,
					"declare -x %s=\"%s\"\n", array_name[i], array_value[i]);
			else
				ft_dprintf(1, "declare -x %s\n", array_name[i]);
		}
	i++;
	}
}

void	swap_str(char ***env_array_name, char ***env_array_value, int j)
{
	char	*tmp;

	tmp = (*env_array_name)[j];
	(*env_array_name)[j] = (*env_array_name)[j + 1];
	(*env_array_name)[j + 1] = tmp;
	tmp = (*env_array_value)[j];
	(*env_array_value)[j] = (*env_array_value)[j + 1];
	(*env_array_value)[j + 1] = tmp;
}

void	print_env_var_ascii(t_env *env)
{
	int		i;
	int		j;
	int		array_size;
	char	**env_array_name;
	char	**env_array_value;

	g_exit_status = 0;
	env_array_name = env_lst_to_array_name(env);
	env_array_value = env_lst_to_array_value(env);
	i = 0;
	array_size = ft_array_size(env_array_name);
	while (i < array_size)
	{
		j = 0;
		while (j < array_size - 1)
		{
			if (ft_strcmp(env_array_name[j], env_array_name[j + 1]) > 0)
				swap_str(&env_array_name, &env_array_value, j);
			j++;
		}
		i++;
	}
	print_sorted(env_array_name, env_array_value);
	ft_free_env_arrays(env_array_name, env_array_value);
}
