void	ft_striteri(char *s, void (*f)(unsigned int, char*))
{
	int	cnt;

	if (!s || !f)
		return ;
	cnt = -1;
	while (s[++cnt])
		f(cnt, &s[cnt]);
}
