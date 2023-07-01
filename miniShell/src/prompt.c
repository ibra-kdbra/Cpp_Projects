#include "../inc/minishell.h"
#include "libft/libft.h"

static void	prompt_get_user(t_env *env);
static void	prompt_get_path(t_env *env);
static void	prompt_get_exitc(int exitc);

void	get_prompt(t_env *env)
{
	prompt_get_user(env);
	prompt_get_path(env);
	prompt_get_exitc(g_exit_status);
	printf("\n");
}

static void	prompt_get_exitc(int exit_stat)
{
	if (exit_stat == 0)
		return ;
	printf(RED"[%d]"NC, exit_stat);
}

static void	prompt_get_path(t_env *env)
{
	t_env	*home;
	char	*pwd;
	char	*tmp;
	int		home_length;

	pwd = getcwd(NULL, 4096);
	home = ft_envlst_search(env, "HOME");
	if (home && ft_strlen(home->var_value) > 0)
	{
		home_length = ft_strlen(home->var_value);
		if (ft_strncmp(pwd, home->var_value, home_length) == 0)
		{
			tmp = ft_substr(pwd, home_length, ft_strlen(pwd) - home_length);
			printf(GREEN"[~%s]"NC, tmp);
			free(pwd);
			free(tmp);
			return ;
		}
	}
	printf(GREEN"[%s]"NC, pwd);
	free(pwd);
}

static void	prompt_get_user(t_env *env)
{
	t_env		*env_tmp;
	char		*tmp;

	env_tmp = ft_envlst_search(env, "USER");
	if (!env_tmp)
	{
		printf(CYAN":"NC);
		return ;
	}
	if (env_tmp->var_value)
		printf(CYAN"%s@"NC, env_tmp->var_value);
	env_tmp = ft_envlst_search(env, "SESSION_MANAGER");
	if (!env_tmp)
		return ;
	tmp = ft_substr(env_tmp->var_value, 6, 3);
	if (tmp)
		printf(CYAN"%s:", tmp);
	free(tmp);
}
