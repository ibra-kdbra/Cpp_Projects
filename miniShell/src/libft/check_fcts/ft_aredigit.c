int	ft_aredigit(char *str, int len)
{
	int	i;

	i = 0;
	while (str[i] && i < len)
	{
		if (!(str[i] >= '0' && str[i] <= '9'))
			return (0);
		i++;
	}
	return (1);
}
