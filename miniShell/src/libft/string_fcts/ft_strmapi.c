#include "../libft.h"

char	*ft_strmapi(char const *s, char (*f)(unsigned int, char))
{
	char	*str;
	int		cnt;

	if (!s || !f)
		return (NULL);
	str = (char *) malloc(ft_strlen(s) + 1);
	if (!str)
		return (NULL);
	cnt = 0;
	while (*s)
	{
		str[cnt] = f(cnt, *s++);
		cnt++;
	}
	str[cnt] = 0;
	return (str);
}
