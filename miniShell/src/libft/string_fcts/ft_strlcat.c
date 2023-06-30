#include "../libft.h"

int	ft_strlcat(char *dst, const char *src, size_t size)
{
	int	len_dst;
	int	len_src;
	int	chrs_to_appnd;

	len_src = ft_strlen(src);
	if (!size && !dst)
		return (len_src);
	len_dst = ft_strlen(dst);
	chrs_to_appnd = size - len_dst - 1;
	dst += len_dst;
	if (len_dst + 1 > (int)size)
		return (size + len_src);
	while (chrs_to_appnd && *src)
	{
		*dst++ = *src++;
		chrs_to_appnd--;
	}
	*dst = 0;
	return (len_dst + len_src);
}
