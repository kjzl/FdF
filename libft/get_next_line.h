/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omed <omed@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 16:40:19 by omed              #+#    #+#             */
/*   Updated: 2024/09/06 16:40:21 by omed             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# include <stdlib.h>
# include <unistd.h>
# include "libft.h"
# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 4242
# endif

typedef struct s_stream
{
	int				fd;
	ssize_t			len;
	size_t			off;
	unsigned char	buff[BUFFER_SIZE];
}	t_stream;

char			*get_next_line(int fd);
void			*ft_memgrow(void *ptr, size_t old_size, size_t new_size);

#endif
