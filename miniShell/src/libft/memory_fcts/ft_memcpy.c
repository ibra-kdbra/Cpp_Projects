#include "../libft.h"

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	char	*dest_cpy;
	char	*src_cpy;

	if (dest == NULL && src == NULL)
		return (NULL);
	dest_cpy = (char *)dest;
	src_cpy = (char *)src;
	while (n)
	{
		*dest_cpy++ = *src_cpy++;
		n--;
	}
	return (dest);
}
