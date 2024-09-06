/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omed <omed@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 16:45:30 by omed              #+#    #+#             */
/*   Updated: 2024/09/06 16:45:31 by omed             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	count_digits(int n)
{
	int	d;

	d = 0;
	if (n < 1)
		++d;
	while (n)
	{
		n /= 10;
		++d;
	}
	return (d);
}

static int	count_udigits(unsigned int n)
{
	int	d;

	d = 0;
	if (!n)
		return (1);
	while (n)
	{
		n /= 10;
		++d;
	}
	return (d);
}

char	*ft_itoa(int n)
{
	char	*num;
	int		digits;
	int		neg;

	digits = count_digits(n);
	num = malloc(digits + 1);
	if (!num)
		return (NULL);
	num[digits] = 0;
	neg = 1;
	if (!n)
		num[0] = '0';
	else if (n < 0)
	{
		num[0] = '-';
		neg = -1;
	}
	while (n && digits--)
	{
		num[digits] = n % 10 * neg + '0';
		n /= 10;
	}
	return (num);
}

char	*ft_uitoa(unsigned int n)
{
	char	*num;
	int		digits;

	digits = count_udigits(n);
	num = malloc(digits + 1);
	if (!num)
		return (NULL);
	num[digits] = 0;
	if (!n)
		num[0] = '0';
	while (n && digits--)
	{
		num[digits] = n % 10 + '0';
		n /= 10;
	}
	return (num);
}
