/** Function that will free ptr[0] to ptr[n] if ptr[n+1] == NULL it will also
 * free ptr **/

#include "../libft.h"

void	ft_free_arr(void **array)
{
	int	i;

	i = 0;
	while (array && array[i])
	{
		free(array[i]);
		i++;
	}
	free(array);
}
