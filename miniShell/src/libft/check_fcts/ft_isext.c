#include "../libft.h"

int	ft_isext(char *file, char *ext)
{
	int	l_ext;
	int	l_file;

	if (!file || !ext)
		return (0);
	l_ext = ft_strlen(ext);
	l_file = ft_strlen(file);
	if (l_ext >= l_file)
		return (0);
	return (ft_strncmp(file + l_file - l_ext, ext, l_ext) == 0
		&& file[l_file - l_ext - 1] != '/');
}
