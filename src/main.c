/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omed <omed@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/14 19:36:45 by omed              #+#    #+#             */
/*   Updated: 2024/10/20 18:52:19 by omed             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minilibx-linux/mlx.h"
#include "../libft/libft.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define ESC_KEY 65307
#define WINDOW_WIDTH 1800
#define WINDOW_HEIGHT 1000

// Keycodes for user input
#define KEY_ESC 65307
#define KEY_PLUS 43
#define KEY_MINUS 45
#define KEY_LEFT 65361
#define KEY_RIGHT 65363
#define KEY_UP 65362
#define KEY_DOWN 65364

// Structure to hold the map data
typedef struct s_map {
    int width;
    int height;
    int **z_matrix;
    int **color_matrix;
} t_map;

// Structure to hold image data
typedef struct s_data {
    void    *img;
    char    *addr;
    int     bits_per_pixel;
    int     line_length;
    int     endian;
} t_data;

// Structure to hold MLX variables and transformation parameters
typedef struct s_vars {
    void    *mlx;
    void    *win;
    t_data  img;
    float   zoom;
    float   zoom_step;
    float   angle;
    float   z_scale;
    int     shift_x;
    int     shift_y;
    t_map   *map; // Correctly typed as t_map *
} t_vars;

// 3D point structure
typedef struct s_point3d {
    float x;
    float y;
    float z;
} t_point3d;

// 2D point structure
typedef struct s_point2d {
    int x;
    int y;
} t_point2d;

// Function to free split arrays
void ft_free_split(char **strs)
{
    int i = 0;
    while (strs[i])
    {
        free(strs[i]);
        i++;
    }
    free(strs);
}

// Function to put a pixel to the image
void my_mlx_pixel_put(t_data *data, int x, int y, int color)
{
    char    *dst;

    // Check if the pixel is within the image boundaries
    if (x >= 0 && x < WINDOW_WIDTH && y >= 0 && y < WINDOW_HEIGHT)
    {
        dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
        *(unsigned int*)dst = color;
    }
}

// Function to get map dimensions
int get_map_dimensions(const char *filename, t_map *map)
{
    int     fd;
    char    *line;
    int     width;

    fd = open(filename, O_RDONLY);
    if (fd < 0)
    {
        perror("Error opening file");
        return (-1);
    }
    map->height = 0;
    map->width = -1;
    while ((line = get_next_line(fd)) != NULL)
    {
        char **nums = ft_split(line, ' ');
        if (!nums)
        {
            free(line);
            close(fd);
            return (-1);
        }
        width = 0;
        while (nums[width])
            width++;
        if (map->width == -1)
            map->width = width;
        else if (map->width != width)
        {
            ft_free_split(nums);
            free(line);
            close(fd);
            fprintf(stderr, "Error: Inconsistent map width\n");
            return (-1);
        }
        ft_free_split(nums);
        free(line);
        map->height++;
    }
    close(fd);
    return (0);
}

// input is a string of a number (for the z_matrix), with an optional comma + hex color
int get_color(char *str)
{
    int color;
    int i;

    i = 0;
    // col = white
    color = 0xFFFFFF;
    while (str[i])
    {
        if (str[i] == ',' && str[i+1] == '0' && str[i+2] == 'x')
        {
            i += 3;
            color = ft_atoi_base(&str[i], "0123456789ABCDEF");
            break;
        }
        i++;
    }
    return (color);
}

// Function to load map data
int load_map(const char *filename, t_map *map)
{
    int     fd;
    char    *line;
    char    **nums;
    int     y;

    if (get_map_dimensions(filename, map) == -1)
        return (-1);

    map->z_matrix = malloc(sizeof(int *) * map->height);
    if (!map->z_matrix)
        return (-1);

    map->color_matrix = malloc(sizeof(int *) * map->height);
    if (!map->color_matrix)
        return (-1);

    fd = open(filename, O_RDONLY);
    if (fd < 0)
    {
        return (-1);
    }

    y = 0;
    while ((line = get_next_line(fd)) != NULL)
    {
        nums = ft_split(line, ' ');
        if (!nums)
        {
            free(line);
            close(fd);
            return (-1);
        }
        map->z_matrix[y] = malloc(sizeof(int) * map->width);
        map->color_matrix[y] = malloc(sizeof(int) * map->width);
        if (!map->z_matrix[y] || !map->color_matrix[y])
        {
            ft_free_split(nums);
            free(line);
            close(fd);
            return (-1);
        }
        for (int x = 0; x < map->width; x++)
        {
            map->z_matrix[y][x] = ft_atoi(nums[x]);
            map->color_matrix[y][x] = get_color(nums[x]);
        }
        ft_free_split(nums);
        free(line);
        y++;
    }
    close(fd);
    return (0);
}

// Function to free map data
void free_map(t_map *map)
{
    for (int y = 0; y < map->height; y++)
    {
        free(map->z_matrix[y]);
        free(map->color_matrix[y]);
    }
    free(map->z_matrix);
    free(map->color_matrix);
}

void rotate_point(t_point3d *point, float angle)
{
    float x_new;
    float y_new;

    x_new = point->x * cos(angle) - point->y * sin(angle);
    y_new = point->x * sin(angle) + point->y * cos(angle);

    point->x = x_new;
    point->y = y_new;
}

// Isometric projection function
void project_iso(t_point3d point, t_point2d *projected, t_vars *vars)
{
	float x;
    float y;
    float z;

    // Apply zoom and z_scale
    point.x *= vars->zoom;
    point.y *= vars->zoom;
    point.z *= vars->zoom / vars->z_scale;

    // Rotate the point around the Z-axis
    rotate_point(&point, vars->angle);

    // Isometric projection formulas
    x = point.x;
    y = point.y;
    z = point.z;

    projected->x = (x - y) * cos(M_PI / 6) + vars->shift_x;
    projected->y = (x + y) * sin(M_PI / 6) - z + vars->shift_y;
}

// Line drawing function (Bresenham's Algorithm)
void draw_line_between_points(t_data *data, t_point2d p1, t_point2d p2, int color)
{
    int dx = abs(p2.x - p1.x);
    int dy = -abs(p2.y - p1.y);
    int sx = (p1.x < p2.x) ? 1 : -1;
    int sy = (p1.y < p2.y) ? 1 : -1;
    int error = dx + dy;
    int error2;

    while (1)
    {
        my_mlx_pixel_put(data, p1.x, p1.y, color);
        if (p1.x == p2.x && p1.y == p2.y)
            break;
        error2 = 2 * error;
        if (error2 >= dy)
        {
            error += dy;
            p1.x += sx;
        }
        if (error2 <= dx)
        {
            error += dx;
            p1.y += sy;
        }
    }
}

t_point2d rect_size(t_point2d max, t_point2d min)
{
    t_point2d size;

    size.x = abs(max.x - min.x);
    size.y = abs(max.y - min.y);
    return (size);
}

t_point2d rect_center_offset(t_point2d a, t_point2d b)
{
    t_point2d offset;

    offset.x = a.x / 2 - b.x / 2;
    offset.y = a.y / 2 - b.y / 2;
    return (offset);
}

t_point2d	point_min(t_point2d a, t_point2d b)
{
	t_point2d min;

	if (a.x < b.x)
		min.x = a.x;
	else
		min.x = b.x;

	if (a.y < b.y)
		min.y = a.y;
	else
		min.y = b.y;

	return (min);
}

t_point2d	point_max(t_point2d a, t_point2d b)
{
	t_point2d max;

	if (a.x > b.x)
		max.x = a.x;
	else
		max.x = b.x;

	if (a.y > b.y)
		max.y = a.y;
	else
		max.y = b.y;

	return (max);
}

t_point2d point_offset(t_point2d point, t_point2d offset)
{
    t_point2d new_point;

    new_point.x = point.x + offset.x;
    new_point.y = point.y + offset.y;
    return (new_point);
}

t_point2d point_neg(t_point2d p)
{
	p.x = -p.x;
	p.y = -p.y;
	return (p);
}

/// returns the on-screen size of the resulting projection
/// and the offset needed for the draw function to start drawing at x=0,y=0
t_point2d    calculate_projection(t_map *map, t_vars *vars, t_point2d *offset_out)
{
	t_point3d	point;
	int			x;
	int			y;
	t_point2d	projected;
	t_point2d	max = {0};
    t_point2d	min = {INT_MAX, INT_MAX};

	x = 0;
	y = 0;
	while (y < map->height)
	{
		x = 0;
		while (x < map->width)
		{
			point = (t_point3d){x, y, .z=map->z_matrix[y][x]};
			project_iso(point, &projected, vars);
			min = point_min(min, projected);
			max = point_max(max, projected);
			if (x < map->width - 1)
			{
				point = (t_point3d){x + 1, y, map->z_matrix[y][x + 1]};
				project_iso(point, &projected, vars);
				min = point_min(min, projected);
				max = point_max(max, projected);
			}
			if (y < map->height - 1)
			{
				point = (t_point3d){x, y + 1, map->z_matrix[y + 1][x]};
				project_iso(point, &projected, vars);
				min = point_min(min, projected);
				max = point_max(max, projected);
			}
			x++;
		}
		y++;
	}
	if (offset_out != NULL)
    	*offset_out = point_neg(min);
    return (rect_size(max, min));
}

// Function to draw the wireframe map
void draw_map(t_data *img, t_map *map, t_vars *vars, t_point2d *offset)
{
	t_point3d	point;
	int			x;
	int			y;
	t_point2d	projected_start;
	t_point2d	projected_end;

	y = 0;
	while (y < map->height)
	{
		x = 0;
		while (x < map->width)
		{
			point = (t_point3d){x, y, .z=map->z_matrix[y][x]};
			project_iso(point, &projected_start, vars);
			projected_start = point_offset(projected_start, *offset);
			if (x < map->width - 1)
			{
				point = (t_point3d){x + 1, y, map->z_matrix[y][x + 1]};
				project_iso(point, &projected_end, vars);
				draw_line_between_points(img, projected_start, point_offset(projected_end, *offset), map->color_matrix[y][x]);
			}
			if (y < map->height - 1)
			{
				point = (t_point3d){x, y + 1, map->z_matrix[y + 1][x]};
				project_iso(point, &projected_end, vars);
				draw_line_between_points(img, projected_start, point_offset(projected_end, *offset), map->color_matrix[y][x]);
			}
			x++;
		}
		y++;
	}
}

void    render_screen(t_vars *vars)
{
    t_point2d center_offset;
    t_point2d content_size;
    t_point2d offset;

     // Redraw the map with updated parameters
    mlx_clear_window(vars->mlx, vars->win);
    if (vars->img.img)
        mlx_destroy_image(vars->mlx, vars->img.img);
    content_size = calculate_projection(vars->map, vars, &offset);
    center_offset = rect_center_offset((t_point2d){WINDOW_WIDTH, WINDOW_HEIGHT}, content_size);
    offset = point_offset(point_offset(center_offset, offset), (t_point2d){vars->shift_x, vars->shift_y});
    vars->img.img = mlx_new_image(vars->mlx, WINDOW_WIDTH, WINDOW_HEIGHT);
    vars->img.addr = mlx_get_data_addr(vars->img.img, &vars->img.bits_per_pixel, &vars->img.line_length, &vars->img.endian);
    draw_map(&vars->img, vars->map, vars, &offset); // Use vars->map
    mlx_put_image_to_window(vars->mlx, vars->win, vars->img.img, 0, 0);
}

void    free_vars(t_vars *vars)
{
    free_map(vars->map);
    if (vars->img.img)
        mlx_destroy_image(vars->mlx, vars->img.img);
    if (vars->win)
        mlx_destroy_window(vars->mlx, vars->win);
    mlx_destroy_display(vars->mlx);
    if (vars->mlx)
        free(vars->mlx);
}

// Key event handler
int handle_key(int keycode, t_vars *vars)
{
    printf("key: %d\n", keycode);
    if (keycode == KEY_ESC)
    {
        free_vars(vars);
        exit(0);
    }
    else if (keycode == KEY_PLUS)
        vars->zoom += vars->zoom_step;
    else if (keycode == KEY_MINUS)
        vars->zoom -= vars->zoom_step;
    else if (keycode == KEY_RIGHT)
        vars->shift_x -= vars->map->width / 10;
    else if (keycode == KEY_LEFT)
        vars->shift_x += vars->map->width / 10;
    else if (keycode == KEY_DOWN)
        vars->shift_y -= vars->map->height / 10;
    else if (keycode == KEY_UP)
        vars->shift_y += vars->map->height / 10;
    else if (keycode == 113) // 'q' key to rotate left by 11,25 degrees
        vars->angle -= M_PI / 16;
    else if (keycode == 101) // 'e' key to rotate right by 11,25 degrees
        vars->angle += M_PI / 16;
    else if (keycode == 97) // 'a' key to decrease z-scale
        vars->z_scale *= 1.1;
    else if (keycode == 100 && vars->z_scale > 0.05) // 'd' key to increase z-scale
        vars->z_scale /= 1.1;

   render_screen(vars);

    return (0);
}

// Mouse event handler
int handle_mouse(int button, int x, int y, t_vars *vars)
{
    // Mark parameters as unused to suppress warnings
    (void)x;
    (void)y;

    if (button == 4) // Scroll up
        vars->zoom += 1;
    else if (button == 5) // Scroll down
        vars->zoom -= 1;

    render_screen(vars);

    return (0);
}

int close_window(t_vars *vars)
{
    free_vars(vars);
    exit(0);  // Exit the program after closing the window
    return (0);
}

// Main function
int main(int argc, char **argv)
{
    t_vars vars;
    t_map map;

    map = (t_map){0};
    if (argc != 2)
    {
        ft_putstr_fd("Usage: ./fdf <map_file>\n", 1);
        return (1);
    }

    if (load_map(argv[1], &map) == -1)
    {
        ft_putstr_fd("Error loading map\n", 1);
        return (1);
    }

    vars.mlx = mlx_init();
    if (!vars.mlx)
    {
        ft_putstr_fd("Error initializing MLX\n", 1);
        free_map(&map);
        return (1);
    }
    vars.win = mlx_new_window(vars.mlx, WINDOW_WIDTH, WINDOW_HEIGHT, "Wireframe Map");
    if (!vars.win)
    {
        ft_putstr_fd("Error creating window\n", 1);
        free_map(&map);
        return (1);
    }

    // Initialize transformation parameters
    vars.img = (t_data){0};
    vars.zoom = 1; // Set initial zoom level
    vars.angle = 0; // Approximately 45 degrees in radians
    vars.z_scale = 1.0;
    vars.shift_x = 0; // Center the map
    vars.shift_y = 0;
    vars.map = &map; // Store map in vars for access in event handlers

    // Calculate the initial zoom level to fit the content
    t_point2d content_size = calculate_projection(&map, &vars, 0);
    float zoom_x = (float)WINDOW_WIDTH / content_size.x;
    float zoom_y = (float)WINDOW_HEIGHT / content_size.y;
    vars.zoom = fmin(zoom_x, zoom_y); // Choose the smaller zoom to fit both dimensions
    vars.zoom_step = vars.zoom / 10; // Set the zoom step to 10% of the initial zoom level

    render_screen(&vars);

    mlx_key_hook(vars.win, handle_key, &vars);
    mlx_mouse_hook(vars.win, handle_mouse, &vars);
    mlx_hook(vars.win, 17, 1L << 17, close_window, &vars);
    mlx_loop(vars.mlx);

    free_map(&map);
    return (0);
}
