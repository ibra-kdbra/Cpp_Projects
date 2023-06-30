#include "../libft.h"

int	ft_lstsize(t_list *lst)
{
	int	cnt;

	if (!lst)
		return (0);
	cnt = 0;
	while (lst)
	{
		cnt++;
		lst = lst->next;
	}
	return (cnt);
}
