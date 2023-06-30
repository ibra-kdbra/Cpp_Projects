int	ft_array_size(char **array_str)
{
	int	i;

	i = 0;
	while (array_str && array_str[i])
		i++;
	return (i);
}
