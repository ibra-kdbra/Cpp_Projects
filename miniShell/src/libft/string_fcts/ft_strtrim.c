#include "../libft.h"

static int	is_set(char c, char const *set)
{
	while (*set)
		if (c == *set++)
			return (1);
	return (0);
}

static int	size_trim(char const *str, char const *set)
{
	int	size;
	int	cnt;
	int	idx_last_chr;

	cnt = 0;
	while (is_set(str[cnt], set))
		cnt++;
	size = cnt - 1;
	idx_last_chr = -1;
	while (str[cnt])
	{
		if (!is_set(str[cnt], set))
			idx_last_chr = cnt;
		cnt++;
	}
	if (idx_last_chr == -1)
		return (-1);
	size = idx_last_chr - size;
	return (size);
}

char	*ft_strtrim(char const *s1, char const *set)
{
	char	*str;
	int		cnt;
	int		size;

	if (!set)
		return ((char *)s1);
	if (!s1 || *s1 == 0 || size_trim(s1, set) == -1)
	{
		str = (char *) malloc(1);
		*str = 0;
		return (str);
	}
	size = size_trim(s1, set);
	str = (char *) malloc(size + 1);
	if (!str)
		return (NULL);
	while (is_set(*s1, set))
		s1++;
	cnt = -1;
	while (++cnt < size)
		*str++ = *s1++;
	*str = 0;
	return (str - size);
}
