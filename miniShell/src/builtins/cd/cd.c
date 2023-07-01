#include "../../../inc/minishell.h"

static bool	check_nb_args(char **args)
{
	if (args[1] == NULL)
	{
		g_exit_status = 1;
		ft_dprintf(2, "minishell: cd: too few arguments\n");
		return (false);
	}
	if (ft_array_size(args) > 2)
	{
		g_exit_status = 1;
		ft_dprintf(2, "minishell: cd: too many arguments\n");
		return (false);
	}
	return (true);
}

static void	set_oldpwd(char *oldpwd, t_env *env)
{
	char	*tmp;
	t_env	*pwdsearch;

	pwdsearch = ft_envlst_search(env, "OLDPWD");
	if (pwdsearch)
		ft_envlst_clearone(&env, pwdsearch);
	if (oldpwd)
	{
		tmp = ft_strjoin("OLDPWD=", oldpwd);
		free(oldpwd);
		ft_envlstadd_back(&env, ft_envlst_new(tmp));
		free(tmp);
	}
	else
		ft_envlstadd_back(&env, ft_envlst_new("OLDPWD"));
}

static void	set_error(char *args, char *old_pwd)
{
	g_exit_status = 1;
	free(old_pwd);
	ft_dprintf(2, "minishell: cd: ");
	perror (args);
}

void	ft_cd(char **args, t_env *env)
{
	char	pwd[4097];
	char	*oldpwd;
	t_env	*pwdsearch;
	t_env	*beforesearch;
	char	*tmp;

	if (!check_nb_args(args))
		return ;
	beforesearch = ft_envlst_search(env, "PWD");
	oldpwd = NULL;
	if (beforesearch && beforesearch->var_value)
		oldpwd = ft_strdup(beforesearch->var_value);
	if (!chdir(args[1]))
	{
		getcwd(pwd, 4096);
		pwdsearch = ft_envlst_search(env, "PWD");
		if (pwdsearch)
			ft_envlst_clearone(&env, pwdsearch);
		tmp = ft_strjoin("PWD=", pwd);
		ft_envlstadd_back(&env, ft_envlst_new(tmp));
		free(tmp);
		set_oldpwd(oldpwd, env);
	}
	else
		set_error(args[1], oldpwd);
}
