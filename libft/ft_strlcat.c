/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omed <omed@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 16:42:24 by omed              #+#    #+#             */
/*   Updated: 2024/09/06 16:42:25 by omed             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t size)
{
	size_t	srclen;
	size_t	dstlen;
	size_t	i;

	srclen = ft_strlen(src);
	dstlen = 0;
	while (dst[dstlen] && dstlen < size)
		++dstlen;
	i = 0;
	if (srclen && size)
	{
		while (*src && i + dstlen < size - 1)
		{
			dst[dstlen + i] = *src++;
			++i;
		}
	}
	if (dstlen + i < size)
		dst[dstlen + i] = 0;
	return (dstlen + srclen);
}
