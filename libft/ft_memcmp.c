/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omed <omed@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 16:44:17 by omed              #+#    #+#             */
/*   Updated: 2024/09/06 16:44:18 by omed             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	int	diff;

	diff = 0;
	while (!diff && n--)
		diff = *(unsigned char *)s1++ - *(unsigned char *)s2++;
	return (diff);
}
