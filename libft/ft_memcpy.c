/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omed <omed@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 16:44:10 by omed              #+#    #+#             */
/*   Updated: 2024/09/06 16:44:11 by omed             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memcpy(void	*dest, const void *src, size_t n)
{
	unsigned char	*ptr;

	ptr = dest;
	if (!src && !dest)
		return (NULL);
	while (n--)
		*ptr++ = *(unsigned char *)src++;
	return (dest);
}
