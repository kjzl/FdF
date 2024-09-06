# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: omed <omed@student.42.fr>                  +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/09/06 16:48:42 by omed              #+#    #+#              #
#    Updated: 2024/09/06 17:06:14 by omed             ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = fdf
#Directories
SRCDIR := src
INCDIR := inc
OBJDIR := obj
LIBFTDIR := libft
LIBMLXDIR := minilibx-linux
#Libraries
LIBFT := $(LIBFTDIR)/libft.a
LIBMLX := $(LIBMLXDIR)/libmlx_Linux.a
#Sources
SRCS := $(addprefix $(SRCDIR)/, main.c)

#Objects
OBJS := $(addprefix $(OBJDIR)/, $(notdir $(SRCS:.c=.o)))

#Header
MLXHEADER := /usr/local/include

#Flags
CC := cc
CFLAGS := -Wall -Wextra -Werror -g -I$(INCDIR) -I$(LIBFTDIR) \
	-I$(LIBMLXDIR) -I$(MLXHEADER)
LDFLAGS := -L$(LIBFTDIR) -L$(LIBMLXDIR)
LDLIBS := -lft -lmlx
LFLAGS := -lbsd -lXext -lX11 -lm

all: $(NAME)

$(NAME): $(HEADERS) $(MLXHEADER) $(LIBFT) $(LIBMLX) $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LDFLAGS) $(LDLIBS) $(LFLAGS)

$(LIBFT): $(LIBFTHEADERS)
	$(MAKE) -C $(LIBFTDIR)

$(LIBMLX):
	$(MAKE) -C $(LIBMLXDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	$(RM) $(OBJS)
	$(RM) -r $(OBJDIR)
	$(MAKE) -C $(LIBFTDIR) clean
	$(MAKE) -C $(LIBMLXDIR) clean

fclean: clean
	$(RM) $(NAME)
	$(MAKE) -C $(LIBFTDIR) fclean

re: fclean all

.PHONY: all clean fclean re
