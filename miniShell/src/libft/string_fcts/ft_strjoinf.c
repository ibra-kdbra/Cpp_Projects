#include "../libft.h"

char	*ft_strjoinf(char *s1, char *s2)
{
	char	*str;
	int		cnt;

	if (!s1)
		return (s2);
	if (!s2)
		return (s1);
	str = ft_calloc(ft_strlen(s1) + ft_strlen(s2) + 1, 1);
	if (!str)
		return (NULL);
	cnt = -1;
	while (s1[++cnt])
		str[cnt] = s1[cnt];
	cnt = -1;
	while (s2[++cnt])
		str[ft_strlen(s1) + cnt] = s2[cnt];
	str[ft_strlen(s1) + cnt] = 0;
	free(s1);
	free(s2);
	return (str);
}
