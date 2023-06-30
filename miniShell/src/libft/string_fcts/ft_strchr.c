#include "../libft.h"

char	*ft_strchr(const char *s, int c)
{
	int		cnt;
	char	*str;

	cnt = 0;
	str = (char *) s;
	while (c > 255)
		c -= 256;
	while (*str)
	{
		if (*str == c)
			return (str + cnt);
		str++;
	}
	if (*str == c)
		return (str);
	return (NULL);
}
