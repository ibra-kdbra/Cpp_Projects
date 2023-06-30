#include "../libft.h"

char	*ft_strrchr(const char *s, int c)
{
	char	*s_cpy;
	char	*last_chr;

	s_cpy = (char *) s;
	last_chr = NULL;
	while (c > 255)
		c -= 256;
	while (*s_cpy)
	{
		if (*s_cpy == c)
			last_chr = s_cpy;
		s_cpy++;
	}
	if (c == 0)
		return (s_cpy);
	return (last_chr);
}
