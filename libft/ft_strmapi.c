/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmapi.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omed <omed@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 16:41:59 by omed              #+#    #+#             */
/*   Updated: 2024/09/06 16:42:01 by omed             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strmapi(const char *s, char (*f)(unsigned int, char))
{
	char	*map;
	size_t	len;

	len = ft_strlen(s);
	map = malloc(len + 1);
	if (!map)
		return (NULL);
	map[len] = 0;
	while (len--)
		map[len] = (*f)((unsigned int)len, s[len]);
	return (map);
}
