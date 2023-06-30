#include "../libft.h"

static int	len_nb(int nb)
{
	int	cnt;

	if (!nb)
		return (1);
	cnt = 0;
	while (nb)
	{
		nb /= 10;
		cnt++;
	}
	return (cnt);
}

static char	*rev_str(char *str, int minus)
{
	char	tmp;
	int		cnt;
	int		len;

	len = ft_strlen(str);
	cnt = 0;
	if (minus)
		str[len] = '-';
	while (cnt < (len + minus) / 2)
	{
		tmp = str[cnt];
		str[cnt] = str[len - 1 - cnt + minus];
		str[len - 1 - cnt + minus] = tmp;
		cnt++;
	}
	return (str);
}

char	*ft_itoa(int n)
{
	char	*str;
	int		minus;
	int		cnt;
	long	na;

	na = n;
	minus = 0;
	if (na < 0)
	{
		minus++;
		na *= -1;
	}
	str = ft_calloc(len_nb(na) + 1 + minus, 1);
	if (!str)
		return (NULL);
	cnt = 0;
	while (na >= 10)
	{
		str[cnt++] = (na % 10) + '0';
		na /= 10;
	}
	str[cnt] = (na % 10) + 48;
	return (rev_str(str, minus));
}
