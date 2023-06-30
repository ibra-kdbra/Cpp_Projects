#include "../libft.h"

static int	lf_check(char *stock)
{
	int	i;

	i = 0;
	if (stock == NULL)
		return (0);
	while (stock[i] != '\0')
	{
		if (stock[i] == '\n')
			return (1);
		i++;
	}
	return (0);
}

static char	*after_lf(char *str, char *keep, int to_free)
{
	int		len;
	char	*after;

	while (*str != '\n' && *(str + 1) != '\0')
		str++;
	str++;
	len = 0;
	while (str[len])
		len++;
	if (!len && to_free)
		free(keep);
	if (!len)
		return (NULL);
	after = ft_calloc(len + 1, 1);
	after[len] = 0;
	len = 0;
	while (str[len])
	{
		after[len] = str[len];
		len++;
	}
	if (to_free)
		free(keep);
	return (after);
}

static char	*before_lf(char	*str)
{
	char	*before;
	int		len;
	int		i;

	len = 0;
	while (str[len] != '\n')
		len++;
	before = ft_calloc(len + 2, 1);
	i = -1;
	before[len + 1] = 0;
	while (++i < len + 1)
		before[i] = str[i];
	return (before);
}

static char	*get_next_line_bis(char **keep, int fd)
{
	char	*line;
	char	*buff;
	int		read_rv;

	line = 0;
	if (*keep && **keep)
		line = *keep;
	buff = ft_calloc(BUFFER_SIZE + 1, 1);
	read_rv = read(fd, buff, BUFFER_SIZE);
	while (read_rv > 0)
	{
		line = ft_strjoinf(line, buff);
		if (lf_check(line))
		{
			*keep = after_lf(line, *keep, 0);
			buff = before_lf(line);
			free(line);
			return (buff);
		}
		buff = ft_calloc(BUFFER_SIZE + 1, 1);
		read_rv = read(fd, buff, BUFFER_SIZE);
	}
	free(buff);
	*keep = 0;
	return (line);
}

char	*get_next_line(int fd)
{
	static char	*keep[1024];
	char		*line;

	if (fd < 0 || fd >= 1024 || BUFFER_SIZE <= 0)
		return (NULL);
	if (lf_check(keep[fd]))
	{
		line = before_lf(keep[fd]);
		keep[fd] = after_lf(keep[fd], keep[fd], 1);
		return (line);
	}
	line = get_next_line_bis(&keep[fd], fd);
	return (line);
}
