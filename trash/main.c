/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omed <omed@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 16:11:14 by omed              #+#    #+#             */
/*   Updated: 2024/09/13 14:19:41 by omed             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minilibx-linux/mlx.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>
#include "../libft/libft.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define ESC_KEY 65307
#define IMG_WIDTH 1000
#define IMG_HEIGHT 2080

typedef struct	s_data {
	void	*img;
	char	*addr;
	int		bits_per_pixel;
	int		line_length;
	int		endian;
}				t_data;


typedef struct  s_vars {
    void *mlx;
    void *win;
} t_vars;

typedef struct  s_point {
    int x;
    int y;
} t_point;


void	my_mlx_pixel_put(t_data *data, int x, int y, int color)
{
	char	*dst;

	dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
	*(unsigned int*)dst = color;
}
int closee(int keycode, t_vars *vars)
{
    if (keycode == ESC_KEY)
    {
        mlx_destroy_window(vars->mlx, vars->win);
        mlx_destroy_display(vars->mlx);
        exit(EXIT_SUCCESS);
       // mlx_destroy_display(vars->mlx);
    }    
    return(0);
}
int close_window(t_vars *vars)
{
    mlx_destroy_window(vars->mlx, vars->win);
    mlx_destroy_display(vars->mlx);
    exit(0);  // Exit the program after closing the window
    return (0);
}

int mouse_hooks(int mousecode, int x, int y)
{
    if (mousecode == 1)
    {
	    printf("Hello from left_mouse_hook at %d %d!\n", x, y);
        
    }

    if (mousecode == 3)
    {
	    printf("Hello from right_mouse_hook at %d %d!\n", x, y);
    }
    return (0);
}

t_point *determine_direction(t_point *p1, t_point *p2)
{
    t_point *direction;

    direction = malloc(sizeof(t_point));
    if (!direction)
        return (NULL);

    if (p1->x < p2->x)
        direction->x = 1;
    else
        direction->x = -1;
    if (p1->y < p2->y)
        direction->y = 1;
    else
        direction->y = -1;
    return (direction);
}

void calculate_next_point(t_point *current, t_point *delta, t_point *direction, int error[2])
{
   error[1] = error[0] * 2;

   if (error[1] > -delta->y)
   {
        error[0] -= delta->y;
        current->x += direction->x;
   }
   
   if (error[1] < delta->x)
   {
        error[0] += delta->x;
        current->y += direction->y;
   }
}

void draw_line_between_points(t_data *data, t_point *p1, t_point *p2, int color)
{
    t_point delta;
    t_point *direction;
    t_point current;
    int error[2];

    delta.x = ft_abs(p2->x - p1->x);
    delta.y = ft_abs(p2->y - p1->y);
    error[0] = delta.x - delta.y;
    current = *p1;
    direction = determine_direction(p1, p2);
    if (!direction)
        return;

    while(current.x != p2->x || current.y != p2-> y)
    {
        my_mlx_pixel_put(data, current.x, current.y, color);
        calculate_next_point(&current, &delta, direction, error);
    }

    my_mlx_pixel_put(data, current.x, current.y, color);
    free(direction);
}


int main()
{
    t_data  img;
    t_vars  vars;
    t_point top_left = {100, 100};  // Top-left corner
    t_point top_right = {200, 100}; // Top-right corner
    t_point bottom_right = {200, 200}; // Bottom-right corner
    t_point bottom_left = {100, 200}; // Bottom-left corner
    int color = 0x00FF0000;   

    vars.mlx = mlx_init();
    vars.win = mlx_new_window(vars.mlx, 1000, 2080, "Hello Omed!");
    img.img = mlx_new_image(vars.mlx, 640, 360);
    img.addr = mlx_get_data_addr(img.img, &img.bits_per_pixel, &img.line_length, &img.endian);
    
    draw_line_between_points(&img, &top_left, &top_right, color);      // Top side
    draw_line_between_points(&img, &top_right, &bottom_right, color);  // Right side
    draw_line_between_points(&img, &bottom_right, &bottom_left, color); // Bottom side
    draw_line_between_points(&img, &bottom_left, &top_left, color);    // Left side
    
	mlx_put_image_to_window(vars.mlx, vars.win, img.img, 0, 0);
    mlx_hook(vars.win, 2, 1L<<0, closee, &vars);
    mlx_hook(vars.win, 17, 1L << 17, close_window, &vars);
    mlx_hook(vars.win, 4, 1L<<2, mouse_hooks, &vars);
    mlx_loop(vars.mlx);
}
