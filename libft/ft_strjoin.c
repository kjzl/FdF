/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omed <omed@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 16:42:32 by omed              #+#    #+#             */
/*   Updated: 2024/09/06 16:42:33 by omed             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin(const char *s1, const char *s2)
{
	char	*joined;
	size_t	len;

	if (!*s2)
		return (ft_strdup(s1));
	if (!*s1)
		return (ft_strdup(s2));
	len = ft_strlen(s1) + ft_strlen(s2) + 1;
	joined = malloc(len);
	if (!joined)
		return (NULL);
	ft_strlcpy(joined, s1, len);
	ft_strlcat(joined, s2, len);
	return (joined);
}
