/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omed <omed@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 16:43:17 by omed              #+#    #+#             */
/*   Updated: 2024/09/06 16:43:18 by omed             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static ssize_t	ft_putn(int n, int fd)
{
	ssize_t	c;

	c = 0;
	if (!n)
		return (0);
	if (n < 0)
	{
		c += ft_putchar_fd('-', fd);
		if (n < -9)
			c += ft_putn(n / 10 * -1, fd);
		c += ft_putchar_fd(n % 10 * -1 + '0', fd);
	}
	else
	{
		if (n > 9)
			c += ft_putn(n / 10, fd);
		c += ft_putchar_fd(n % 10 + '0', fd);
	}
	return (c);
}

ssize_t	ft_putnbr_fd(int n, int fd)
{
	if (!n)
		return (ft_putchar_fd('0', fd));
	else
		return (ft_putn(n, fd));
}
