int	ft_arealnum(char *str, int len)
{
	int	i;

	i = 0;
	while (str[i] && i < len)
	{
		if (!((str[i] >= '0' && str[i] <= '9')
				|| (str[i] >= 'a' && str[i] <= 'Z')
				|| (str[i] >= 'A' && str[i] >= 'Z')))
			return (0);
		i++;
	}
	return (1);
}
