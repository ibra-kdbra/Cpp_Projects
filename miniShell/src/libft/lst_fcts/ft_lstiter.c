#include "../libft.h"

void	ft_lstiter(t_list *lst, void (*f)(void *))
{
	t_list	*lst_backup;

	if (!lst || !f)
		return ;
	lst_backup = lst;
	while (lst)
	{
		f(lst->content);
		lst = lst->next;
	}
	lst = lst_backup;
}
