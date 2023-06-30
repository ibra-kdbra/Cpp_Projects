#include "../libft.h"

static int	nb_words(char const *s, char c)
{
	int	cnt;

	cnt = 0;
	if (*s != c && *s)
	{
		cnt++;
		s++;
	}
	while (*s)
	{
		if (*s != c && *(s - 1) == c)
			cnt++;
		s++;
	}
	return (cnt);
}

static int	len_nxt_wd(char const *s, char c)
{
	int	cnt;

	cnt = 0;
	while (*s != c && *s)
	{
		cnt++;
		s++;
	}
	return (cnt);
}

static void	*free_arr(char **arr)
{
	int	i;

	i = 0;
	while (!arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
	return (NULL);
}

char	**ft_split(char const *s, char c)
{
	char	**arr;
	int		nb_wd;
	int		j;
	int		i;

	if (!s)
		return (NULL);
	nb_wd = nb_words(s, c);
	arr = (char **)ft_calloc((nb_wd + 1), sizeof(char *));
	if (!arr)
		return (NULL);
	j = 0;
	while (j < nb_wd)
	{
		while (*s == c && *s)
			s++;
		arr[j] = (char *) malloc(len_nxt_wd(s, c) + 1);
		if (!arr[j])
			return (free_arr(arr));
		i = 0;
		while (*s != c && *s)
			arr[j][i++] = *s++;
		arr[j++][i] = 0;
	}
	return (arr);
}
