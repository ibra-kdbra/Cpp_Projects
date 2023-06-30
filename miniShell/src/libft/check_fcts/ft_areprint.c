int	ft_areprint(char *str, int len)
{
	int	i;

	i = 0;
	while (str[i] && i < len)
	{
		if (!(str[i] >= 32 && str[i] <= 126))
			return (0);
		i++;
	}
	return (1);
}
