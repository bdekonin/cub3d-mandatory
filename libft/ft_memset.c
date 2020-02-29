/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ft_memset.c                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: bdekonin <bdekonin@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2019/11/01 15:24:25 by bdekonin       #+#    #+#                */
/*   Updated: 2019/11/27 13:59:42 by bdekonin      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memset(void *b, int c, size_t n)
{
	char	*ptr;
	size_t	i;

	ptr = b;
	i = 0;
	while (i < n)
	{
		ptr[i] = c;
		i++;
	}
	return (b);
}
