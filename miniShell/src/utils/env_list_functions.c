#include "../../inc/minishell.h"

t_env	*ft_envlst_last(t_env *env)
{
	t_env	*last;

	last = env;
	if (env == NULL)
		return (NULL);
	while (env != NULL)
	{
		last = env;
		env = env->next;
	}
	return (last);
}

void	ft_envlstadd_back(t_env **env, t_env *new_env)
{
	t_env	*last;

	if (!new_env || !env)
		return ;
	if (*env == NULL)
		*env = new_env;
	else
	{
		last = ft_envlst_last(*env);
		last->next = new_env;
		new_env->prev = last;
	}
}

static void	create_and_assign(char *str, t_env **new)
{
	int	i;
	int	var_value_size;

	i = -1;
	while (str[++i])
	{
		if (str[i] == '=')
			break ;
	}
	if (str[i - 1] == '+')
	{
		(*new)->var_name = ft_calloc(sizeof(char), i);
		ft_strlcpy((*new)->var_name, str, i);
	}
	else
	{
		(*new)->var_name = ft_calloc(sizeof(char), i + 1);
		ft_strlcpy((*new)->var_name, str, i + 1);
	}
	var_value_size = ft_strlen(str) - ft_strlen((*new)->var_name);
	(*new)->var_value = ft_calloc(sizeof(char), var_value_size);
	ft_strlcpy((*new)->var_value, str + i + 1, var_value_size);
}

t_env	*ft_envlst_new(char *str)
{
	t_env	*new;

	new = ft_calloc(sizeof(t_env), 1);
	if (!new)
		return (0);
	if (!ft_strchr(str, '='))
	{
		new->var_name = ft_strdup(str);
		if (!new->var_name)
			return (0);
		new->var_value = NULL;
	}
	else
		create_and_assign(str, &new);
	return (new);
}

t_env	*get_env_variables(char **envp)
{
	int		i;
	t_env	*root;
	t_env	*new;

	i = 0;
	root = NULL;
	while (envp[i])
	{
		new = ft_envlst_new(envp[i]);
		ft_envlstadd_back(&root, new);
		i++;
	}
	return (root);
}
