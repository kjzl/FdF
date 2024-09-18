#ifndef FDF_H
# define FDF_H

# include <math.h>
# include "../minilibx-linux/mlx.h"
# include "../libft/libft.h"
# include <unistd.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <stdio.h>
# include <stdlib.h>
# include <stdbool.h>

typedef struct s_img_data
{
	int		pixel_bits;
	int		line_bytes;
	int		endian;
	char	*img;
}	t_img_data;

typedef struct s_fdf
{
	t_img_data	data;
	float		x1;
	float		y1;
	void		*img;
	void		*p_mlx;
	void		*p_win;
	char		**ag;
	int			height;
	int			width;
	int			**map;
	float		zoom;
	int			h_move;
	int			v_move;
	float		h_view;
}		t_fdf;

int		find_mod(int num);
int		find_max(int num1, int num2);
void	ft_free_tab(char **tab);
void	put_pxl(t_fdf *tab, int x, int y, int color);
char	*get_next_line(int fd);
int		ft_isdigit(int character);
void	readfile(t_fdf *tab);
void	tracing(t_fdf *tab);
void	ft_free_tab_of_int(int **tab);

#endif


void	ft_free_tab(char **tab)
{
	size_t	i;

	i = 0;
	while (tab[i])
	{
		free(tab[i]);
		i++;
	}
	free(tab);
}

int	find_max(int num1, int num2)
{
	if (num1 > num2)
		return (num1);
	else
		return (num2);
}

int	find_mod(int num)
{
	if (0 > num)
		return (-num);
	return (num);
}

int	getheight(t_fdf *tab)
{
	int		fd;
	char	*str;
	int		i;

	fd = open(tab->ag[1], O_RDONLY, 0777);
	i = 0;
	str = get_next_line(fd);
	while (str)
	{
		i++;
		free(str);
		str = get_next_line(fd);
	}
	close(fd);
	return (i);
}

int	getwidth(t_fdf *tab)
{
	int		fd;
	char	**s_str;
	int		i;
	char	*gnl;

	fd = open(tab->ag[1], O_RDONLY, 0777);
	if (fd == -1)
		exit(-1);
	gnl = get_next_line(fd);
	s_str = ft_split(gnl, ' ');
	i = 0;
	if (s_str == NULL)
		exit(-1);
	while (s_str[i])
		i++;
	ft_free_tab(s_str);
	free(gnl);
	close(fd);
	return (i);
}

int	**mallocfile(t_fdf *tab)
{
	int	**res;
	int	i;

	tab->width = getwidth(tab);
	tab->height = getheight(tab);
	res = malloc(sizeof(int *) * tab->height);
	if (!res)
		exit(-1);
	i = 0;
	while (i < tab->height)
	{
		res[i] = malloc(sizeof(int) * tab->width);
		if (!res[i])
			exit(-1);
		i++;
	}
	return (res);
}

void	readfile(t_fdf *tab)
{
	char	*str;
	char	**s_str;
	int		fd;
	int		j;
	int		i;

	tab->map = mallocfile(tab);
	fd = open(tab->ag[1], O_RDONLY);
	str = get_next_line(fd);
	j = 0;
	while (str)
	{
		s_str = ft_split(str, ' ');
		i = 0;
		while (i < tab->width)
		{
			tab->map[j][i] = ft_atoi(s_str[i]);
			i++;
		}
		free(str);
		ft_free_tab(s_str);
		str = get_next_line(fd);
		j++;
	}
	close(fd);
}
int	fade(int h)
{
	if (h > 100)
		return (0xFFDF8D);
	if (h > 75)
		return (0xFFDE7A);
	if (h > 50)
		return (0xFFC568);
	if (h > 25)
		return (0xFD996B);
	if (h > 15)
		return (0xF7856C);
	if (h > 10)
		return (0xF06E6C);
	if (h > 5)
		return (0xD9576B);
	if (h > 0)
		return (0xA44369);
	if (h > -10)
		return (0x833F68);
	if (h > -20)
		return (0x833F68);
	if (h > -50)
		return (0x5E3C65);
	return (0x3F3A63);
}

void	projection(float *x, float *y, int *z, t_fdf *tab)
{
	//int	x_tmp;
	//int	y_tmp;

	*z = tab->map[(int)*y][(int)*x];
	*z *= tab->zoom;
	*z *= tab->h_view;
	*y *= tab->zoom;
	*x *= tab->zoom;
	//y_tmp = *y;
	//x_tmp = *x;
	*x = (*x - *y) * cos(0.8);
	*y = (*x + *y) * sin(0.8) - *z;
}

void	put_pxl(t_fdf *tab, int x, int y, int color)
{
	int		i;

	i = (x * tab->data.pixel_bits / 8) + (y * tab->data.line_bytes);
	tab->data.img[i] = color;
	tab->data.img[++i] = color >> 8;
	tab->data.img[++i] = color >> 16;
}

void	trace_line(float x0, float y0, t_fdf *tab)
{
	int		z0;
	int		z1;
	float	x_step;
	float	y_step;
	int		max;

	projection(&x0, &y0, &z0, tab);
	projection(&tab->x1, &tab->y1, &z1, tab);
	x0 += tab->h_move;
	y0 += tab->v_move;
	tab->x1 += tab->h_move;
	tab->y1 += tab->v_move;
	x_step = tab->x1 - x0;
	y_step = tab->y1 - y0;
	max = find_max(find_mod(x_step), find_mod(y_step));
	x_step /= max;
	y_step /= max;
	while ((int)(x0 - tab->x1) || (int)(y0 - tab->y1))
	{
		if (x0 < 1000 && y0 < 800 && x0 > 0 && y0 > 0)
			put_pxl(tab, x0, y0, fade(find_max(z0, z1)));
		x0 += x_step;
		y0 += y_step;
	}
}

void	tracing(t_fdf *tab)
{
	int	i;
	int	j;

	i = 0;
	while (i < tab->height)
	{
		j = 0;
		while (j < tab->width)
		{
			if (j < tab->width - 1)
			{
				tab->x1 = j + 1;
				tab->y1 = i;
				trace_line(j, i, tab);
			}
			if (i < tab->height - 1)
			{
				tab->x1 = j;
				tab->y1 = i + 1;
				trace_line(j, i, tab);
			}
			j++;
		}
		i++;
	}
}
void	draw_background(t_fdf *tab, int color)
{
	int	x;
	int	y;

	y = 0;
	while (++y < 800)
	{
		x = 0;
		while (++x < 1000)
			put_pxl(tab, x, y, color);
	}
}

void	pannel(t_fdf *tab)
{
	mlx_string_put (tab->p_mlx, tab->p_win, 20, 10, 0xFFFFFF, "<< COMMANDS >>");
	mlx_string_put (tab->p_mlx, tab->p_win, 20, 30, 0xFFFFFF, "W - Move up");
	mlx_string_put (tab->p_mlx, tab->p_win, 20, 50, 0xFFFFFF, "S - Move down");
	mlx_string_put (tab->p_mlx, tab->p_win, 20, 70, 0xFFFFFF, "A - Move left");
	mlx_string_put (tab->p_mlx, tab->p_win, 20, 90,
		0xFFFFFF, "D - Move right");
	mlx_string_put(tab->p_mlx, tab->p_win, 20, 110,
		0xFFFFFF, "R - Increase depth");
	mlx_string_put (tab->p_mlx, tab->p_win, 20, 130,
		0xFFFFFF, "F - Decrease depth");
}

void	show_map(t_fdf *tab)
{
	mlx_clear_window(tab->p_mlx, tab->p_win);
	tab->img = mlx_new_image(tab->p_mlx, 1000, 800);
	tab->data.img = mlx_get_data_addr(tab->img, &tab->data.pixel_bits,
			&tab->data.line_bytes, &tab->data.endian);
	draw_background(tab, 0x181C26);
	tracing(tab);
	mlx_put_image_to_window(tab->p_mlx, tab->p_win, tab->img, 0, 0);
	pannel(tab);
}

int	event(int key, t_fdf *tab)
{
	if (key == 65307)
	{
		mlx_destroy_window(tab->p_mlx, tab->p_win);
		exit(0);
	}
	if (key == 97)
		tab->h_move += 20;
	if (key == 100)
		tab->h_move -= 20;
	if (key == 119)
		tab->v_move += 20;
	if (key == 115)
		tab->v_move -= 20;
	if (key == 114)
		tab->h_view += 0.01;
	if (key == 102)
		tab->h_view -= 0.01;
	show_map(tab);
	return (0);
}

int	main(int ac, char **ag)
{
	t_fdf	*tab;

	if (ac != 2)
		exit(-1);
	tab = malloc(sizeof(t_fdf));
	if (!tab)
		exit(-1);
	tab->h_view = 0.01;
	tab->h_move = 500;
	tab->v_move = 50;
	tab->ag = ag;
	tab->p_mlx = mlx_init();
	readfile(tab);
	tab->p_win = mlx_new_window(tab->p_mlx, 1000, 800, ag[1]);
	tab->zoom = find_max(1000 / tab->width, 2);
	show_map(tab);
	mlx_do_key_autorepeaton(tab->p_mlx);
	mlx_hook(tab->p_win, 2, (1L << 0), event, tab);
	mlx_loop(tab->p_mlx);
}