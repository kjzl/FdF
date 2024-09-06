/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omed <omed@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 16:41:16 by omed              #+#    #+#             */
/*   Updated: 2024/09/06 16:56:56 by omed             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	is_set(const char c, const char *set)
{
	if (!set)
		return (0);
	if (!c)
		return (1);
	while (*set)
		if (c == *set++)
			return (1);
	return (0);
}

char	*ft_strtrim(const char *s1, const char *set)
{
	char	*trim;
	int		start;
	int		end;
	size_t	len;

	start = 0;
	while (s1[start] && is_set(s1[start], set))
		++start;
	end = ft_strlen(s1);
	while (end-- > start && is_set(s1[end], set))
		;
	len = 0;
	if (end >= start)
		len = (size_t)(end - start + 1);
	trim = ft_substr(s1, start, len);
	return (trim);
}
