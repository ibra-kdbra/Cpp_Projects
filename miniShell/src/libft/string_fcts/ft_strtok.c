#include "../libft.h"

char	*ft_strtok(char *str, const char *delim)
{
	static char	*buf = NULL;

	if (str)
		buf = str;
	if (!buf || !delim)
		return (NULL);
	while (*buf && ft_strchr(delim, *buf))
		buf++;
	str = buf;
	if (!*str)
		return (NULL);
	while (*buf && !ft_strchr(delim, *buf))
		buf++;
	if (*buf)
		*buf++ = 0;
	if (!*buf)
		buf = NULL;
	return (str);
}
