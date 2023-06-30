#include "../libft.h"

char	*ft_strjoin_add_space(char const *s1, char const *s2)
{
	char	*new;
	int		i;
	int		j;

	i = 0;
	j = 0;
	if (!s1 || !s2)
		return (NULL);
	new = ft_calloc(sizeof(char), (ft_strlen(s1) + ft_strlen(s2) + 2));
	if (!new)
		return (0);
	while (s1[i])
	{
		new[i] = s1[i];
		i++;
	}
	new[i] = 32;
	i++;
	while (s2[j])
	{
		new[i] = s2[j];
		i++;
		j++;
	}
	return (new);
}
