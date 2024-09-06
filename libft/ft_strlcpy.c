/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omed <omed@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 16:42:18 by omed              #+#    #+#             */
/*   Updated: 2024/09/06 16:42:19 by omed             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcpy(char *dst, const char *src, size_t size)
{
	size_t	srclen;
	size_t	i;

	srclen = ft_strlen(src);
	if (size && src && dst)
	{
		if (size > srclen)
			size = srclen + 1;
		i = 0;
		while (i < size - 1)
		{
			dst[i] = src[i];
			++i;
		}
		dst[i] = 0;
	}
	return (srclen);
}
