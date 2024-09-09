/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omed <omed@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 16:11:14 by omed              #+#    #+#             */
/*   Updated: 2024/09/07 16:50:55 by omed             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minilibx-linux/mlx.h"

#define ESC_KEY 65307

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

void	my_mlx_pixel_put(t_data *data, int x, int y, int color)
{
	char	*dst;

	dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
	*(unsigned int*)dst = color;
}
int close(int keycode, t_vars *vars)
{
    if (keycode == ESC_KEY)
        mlx_destroy_window(vars->mlx, vars->win);
    return(0);
}


int main()
{
    t_data  img;
    t_vars  vars;

    vars.mlx = mlx_init();
    vars.win = mlx_new_window(vars.mlx, 1000, 2080, "Hello Omed!");
    img.img = mlx_new_image(vars.mlx, 640, 360);
    img.addr = mlx_get_data_addr(img.img, &img.bits_per_pixel, &img.line_length, &img.endian);
    
    int x = 10;
    int y = 10;
    int a = 100;
    int b = 100;
    while( x <= 100 && y <= 100)
    {
        my_mlx_pixel_put(&img, x, y, 0x00FF0000);
        my_mlx_pixel_put(&img, a, b, 0x00FF0000);
	    mlx_put_image_to_window(vars.mlx, vars.win, img.img, 0, 0);
        x++;
        y++;
        a--;
        b++;
    }
    mlx_hook(vars.win, 2, 1L<<0, close, &vars);
    mlx_loop(vars.mlx);
}
