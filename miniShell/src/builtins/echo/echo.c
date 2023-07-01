#include "../../../inc/minishell.h"

static bool	contain_only_n(const char *str)
{
	int	i;

	if (!str || !str[0] || !str[1])
		return (false);
	i = 1;
	while (str[i])
	{
		if (str[i] != 'n')
			return (false);
		i++;
	}
	return (true);
}

static void	execute_echo_noflag(char **array_str)
{
	int		i;
	char	*tmp;
	char	*output;

	i = 1;
	output = ft_strdup(array_str[1]);
	while (array_str[i + 1])
	{
		tmp = ft_strdup(output);
		free (output);
		output = ft_strjoin_add_space(tmp, array_str[i + 1]);
		free (tmp);
		i++;
	}
	printf("%s\n", output);
	free (output);
	return ;
}

static void	execute_echo_withflag(char **array_str, int size)
{
	int		i;
	char	*tmp;
	char	*output;

	i = 1;
	while (contain_only_n(array_str[i]))
	{
		if (i == size - 1)
			return ;
		i++;
	}
	output = ft_strdup(array_str[i]);
	while (array_str[i + 1])
	{
		tmp = ft_strdup(output);
		free (output);
		output = ft_strjoin_add_space(tmp, array_str[i + 1]);
		free (tmp);
		i++;
	}
	printf("%s", output);
	free (output);
	return ;
}

void	ft_echo(char **array_str)
{
	int	size;

	g_exit_status = 0;
	size = ft_array_size(array_str);
	if (size == 1)
	{
		printf("\n");
		return ;
	}
	if (array_str[1][0] != '-' ||
			(array_str[1][0] == '-' && ft_strlen(array_str[1]) == 1) ||
			!contain_only_n(array_str[1]))
		execute_echo_noflag(array_str);
	else if (contain_only_n(array_str[1]))
		execute_echo_withflag(array_str, size);
}
