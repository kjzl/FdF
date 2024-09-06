/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omed <omed@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 16:42:59 by omed              #+#    #+#             */
/*   Updated: 2024/09/06 16:43:00 by omed             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	count_words(const char *s, char c)
{
	int	count;
	int	word;

	count = 0;
	word = 0;
	while (*s)
	{
		if (*s++ == c)
		{
			if (word)
			{
				++count;
				word = 0;
			}
		}
		else
			++word;
	}
	if (word)
		++count;
	return (count);
}

static char	*get_word(const char *s, int *size)
{
	char	*word;

	word = ft_substr(s, 0, *size);
	*size = 0;
	return (word);
}

static void	graciously_exit(char **spl, int i)
{
	while (i--)
		free(spl[i]);
	free(spl);
}

char	**ft_split(const char *s, char c)
{
	char	**spl;
	int		count;
	int		word;
	int		i;

	count = count_words(s, c);
	spl = malloc(sizeof(char *) * (count + 1));
	if (!spl)
		return (NULL);
	i = 0;
	word = 0;
	while (i < count)
	{
		if ((*s == c || !*s) && word)
		{
			spl[i] = get_word(s - word, &word);
			if (!spl[i++])
				return (graciously_exit(spl, i), NULL);
		}
		else if (*s != c)
			++word;
		++s;
	}
	spl[i] = NULL;
	return (spl);
}
