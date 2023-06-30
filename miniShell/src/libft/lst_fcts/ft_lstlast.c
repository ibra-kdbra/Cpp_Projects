#include "../libft.h"

t_list	*ft_lstlast(t_list *lst)
{
	t_list	*ptr_last;

	if (!lst)
		return (NULL);
	while (lst)
	{
		ptr_last = lst;
		lst = lst->next;
	}
	return (ptr_last);
}
