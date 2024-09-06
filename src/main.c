/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omed <omed@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 16:11:14 by omed              #+#    #+#             */
/*   Updated: 2024/09/06 17:21:15 by omed             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minilibx-linux/mlx.h"

int main()
{
    void    *mlx;
    void    *mlx_win;
    void    *img;

    mlx = mlx_init();
    mlx_win = mlx_new_window(mlx, 1920, 1080, "Hello Omed!");
    img = mlx_new_image(mlx, 640, 360);
    mlx_loop(mlx);
}
