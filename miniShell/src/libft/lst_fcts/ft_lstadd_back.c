#include "../libft.h"

void	ft_lstadd_back(t_list **lst, t_list *new)
{
	t_list	*lst_backup;

	if (!*lst)
	{
		*lst = new;
		return ;
	}
	lst_backup = *lst;
	while ((*lst)->next)
		*(lst) = (*lst)->next;
	(*lst)->next = new;
	*lst = lst_backup;
}
