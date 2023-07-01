#include "../../inc/minishell.h"

char	**env_lst_to_array_name(t_env *env)
{
	int		i;
	char	**env_array_name;

	i = 0;
	env_array_name = malloc(sizeof(char *) * (ft_envlst_size(env) + 1));
	if (!env_array_name)
	{
		perror("malloc");
		return (NULL);
	}
	while (env != NULL)
	{
		env_array_name[i] = ft_strdup(env->var_name);
		env = env->next;
		i++;
	}
	env_array_name[i] = NULL;
	return (env_array_name);
}

char	**env_lst_to_array_value(t_env *env)
{
	char	**env_array_value;
	int		i;

	i = 0;
	env_array_value = malloc(sizeof(char *) * (ft_envlst_size(env) + 1));
	if (!env_array_value)
	{
		perror("malloc");
		return (NULL);
	}
	while (env != NULL)
	{
		if (env->var_value)
			env_array_value[i] = ft_strdup(env->var_value);
		else
			env_array_value[i] = NULL;
		env = env->next;
		i++;
	}
	env_array_value[i] = NULL;
	return (env_array_value);
}
