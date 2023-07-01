#include "../../inc/minishell.h"
#include <dirent.h>

bool	check_dir(char *cmd)
{
	DIR	*dirp;

	if (!cmd)
		return (false);
	dirp = opendir(cmd);
	if (dirp)
	{
		ft_dprintf(2, "minishell: %s: Is a directory\n", cmd);
		g_exit_status = 126;
		closedir(dirp);
		return (true);
	}
	return (false);
}
