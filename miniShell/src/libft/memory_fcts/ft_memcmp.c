#include "../libft.h"

int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	const unsigned char	*s1_cpy;
	const unsigned char	*s2_cpy;

	s1_cpy = s1;
	s2_cpy = s2;
	while (n)
	{
		if (*s1_cpy != *s2_cpy)
			return (*s1_cpy - *s2_cpy);
		s1_cpy++;
		s2_cpy++;
		n--;
	}
	return (0);
}
