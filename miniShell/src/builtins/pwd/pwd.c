#include "../../../inc/minishell.h"

void	ft_pwd(void)
{
	char	buffer[4097];

	g_exit_status = 0;
	getcwd(buffer, 4096);
	ft_dprintf(1, "%s\n", buffer);
}
