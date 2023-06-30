#include "../libft.h"

int	ft_putstr_fd(char *str, int fd)
{
	int	cnt;

	cnt = 0;
	if (!str)
	{
		write(fd, "(null)", 6);
		return (6);
	}	
	while (str[cnt])
	{
		write(fd, &str[cnt], 1);
		cnt++;
	}
	return (cnt);
}
