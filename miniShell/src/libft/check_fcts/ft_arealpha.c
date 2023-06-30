int	ft_arealpha(char *str, int len)
{
	int	i;

	i = 0;
	while (str[i] && i < len)
	{
		if (!((str[i] >= 'a' && str[i] <= 'z')
				|| (str[i] >= 'A' && str[i] <= 'Z')))
			return (0);
		i++;
	}
	return (1);
}
