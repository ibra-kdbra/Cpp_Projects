int	ft_areblank(char *str, int len)
{
	int	i;

	i = 0;
	while (str[i] && i < len)
	{
		if (!(str[i] == ' ' || str[i] == '\t'))
			return (0);
		i++;
	}
	return (1);
}
