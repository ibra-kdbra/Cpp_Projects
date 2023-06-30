#include "../libft.h"

void	*ft_memset(void *s, int c, size_t n)
{
	int		i;
	char	*str;

	i = 0;
	str = (char *) s;
	while (n > 0)
	{
		str[i] = c;
		i++;
		n--;
	}
	s = (void *) str;
	return (s);
}
