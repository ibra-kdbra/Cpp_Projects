#include "../libft.h"

char	*ft_strdup(const char *s)
{
	char	*dup;
	int		cnt;
	int		len;

	len = ft_strlen(s);
	dup = (char *)malloc(len + 1);
	if (!dup)
		return (NULL);
	cnt = -1;
	while (s[++cnt])
		dup[cnt] = s[cnt];
	dup[cnt] = 0;
	return (dup);
}
