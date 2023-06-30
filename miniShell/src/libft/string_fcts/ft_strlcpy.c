#include "../libft.h"

int	ft_strlcpy(char *dst, const char *src, size_t size)
{
	int	cnt;

	cnt = 0;
	while (cnt + 1 < (int)size && src[cnt])
	{
		dst[cnt] = src[cnt];
		cnt++;
	}
	if (size)
		dst[cnt] = 0;
	return (ft_strlen(src));
}
