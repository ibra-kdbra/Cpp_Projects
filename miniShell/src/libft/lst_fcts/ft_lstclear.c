#include "../libft.h"

void	ft_lstclear(t_list **lst, void (*del)(void*))
{
	t_list	*lst_backup;

	if (!lst || !del)
		return ;
	while (*lst)
	{
		lst_backup = (*lst)->next;
		ft_lstdelone(*lst, del);
		*lst = lst_backup;
	}
}
