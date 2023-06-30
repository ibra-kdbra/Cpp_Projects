#include "../libft.h"

int	ft_putendl_fd(char *s, int fd)
{
	int	cnt;

	cnt = ft_putstr_fd(s, fd);
	if (s)
	{
		write(fd, "\n", 1);
		return (cnt + 1);
	}
	return (cnt);
}
