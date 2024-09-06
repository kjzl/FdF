/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omed <omed@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 16:41:42 by omed              #+#    #+#             */
/*   Updated: 2024/09/06 16:41:44 by omed             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *big, const char *little, size_t len)
{
	size_t	llen;
	size_t	i;

	if (!*little)
		return ((char *)big);
	llen = 0;
	while (little[llen])
		++llen;
	i = 0;
	while (big[i] && i + llen <= len)
	{
		if (!ft_strncmp(&big[i], little, llen))
			return ((char *)&big[i]);
		++i;
	}
	return (NULL);
}
