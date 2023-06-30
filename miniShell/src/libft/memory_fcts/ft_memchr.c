#include "../libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	char					*s_cpy;
	long unsigned int		cnt;

	if (!s)
		return (NULL);
	s_cpy = (char *)s;
	while (c > 255)
		c -= 256;
	cnt = 0;
	while (cnt < n)
	{
		if (s_cpy[cnt] == c)
			return (s_cpy + cnt);
		cnt++;
	}
	return (NULL);
}
