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

// Isometric projection function
void project_iso(t_point3d point, t_point2d *projected, t_vars *vars)
{
    float x;
    float y;
    float z;

    x = point.x * vars->zoom;
    y = point.y * vars->zoom;
    z = point.z * vars->zoom / vars->z_scale;

    // Isometric projection formulas
    projected->x = (x - y) * cos(vars->angle) + vars->shift_x;
    projected->y = (x + y) * sin(vars->angle) - z + vars->shift_y;
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

    printf("max.x: %d, max.y: %d, b.x: %d, b.y: %d\n", max.x, max.y, min.x, min.y);
    size.x = abs(max.x - min.x);
    size.y = abs(max.y - min.y);
    printf("size.x: %d, size.y: %d\n", size.x, size.y);
    return (size);
}

t_point2d rect_offset_center(t_point2d big, t_point2d little)
{
    t_point2d offset;

    // big = 1000 * 800
    // little = 390 * 544
    // offset = 305, 128
    // eigentlich = 
    offset.x = big.x / 2 - little.x / 2;
    offset.y = big.y / 2 - little.y / 2;
    printf("render offset.x: %d, offset.y: %d\n", offset.x, offset.y);
    return (offset);
}

t_point2d    calculate_img(t_map *map, t_vars *vars, t_point2d *offset)
{
    t_point3d point;
    t_point2d projected_start, projected_end;
    t_point2d max = {0};
    t_point2d min = {INT_MAX, INT_MAX};
    for (int y = 0; y < map->height; y++)
    {
        for (int x = 0; x < map->width; x++)
        {
            point.x = x;
            point.y = y;
            point.z = map->z_matrix[y][x];

            project_iso(point, &projected_start, vars);
            if (min.x > projected_start.x)
                min.x = projected_start.x;
            if (min.y > projected_start.y)
                min.y = projected_start.y;
            if (max.x < projected_start.x)
                max.x = projected_start.x;
            if (max.y < projected_start.y)
                max.y = projected_start.y;

            if (x < map->width - 1)
            {
                t_point3d point_right = {x + 1, y, map->z_matrix[y][x + 1]};
                project_iso(point_right, &projected_end, vars);
            }

            if (y < map->height - 1)
            {
                t_point3d point_down = {x, y + 1, map->z_matrix[y + 1][x]};
                project_iso(point_down, &projected_end, vars);
            }
            if (min.x > projected_end.x)
                    min.x = projected_end.x;
                if (min.y > projected_end.y)
                    min.y = projected_end.y;
                if (max.x < projected_end.x)
                    max.x = projected_end.x;
                if (max.y < projected_end.y)
                    max.y = projected_end.y;
        }
    }
    *offset = min;
    return (rect_size(max, min));
}

t_point2d point_offset(t_point2d point, t_point2d offset)
{
    t_point2d new_point;

    new_point.x = point.x - offset.x;
    new_point.y = point.y - offset.y;
    return (new_point);
}

// Function to draw the wireframe map
void draw_map(t_data *img, t_map *map, t_vars *vars, t_point2d *offset)
{
    t_point3d point;
    t_point2d projected_start, projected_end;
    //int color = 0xFFFFFF; // White color for lines

    for (int y = 0; y < map->height; y++)
    {
        for (int x = 0; x < map->width; x++)
        {
            point.x = x;
            point.y = y;
            point.z = map->z_matrix[y][x];

            project_iso(point, &projected_start, vars);
            if (x < map->width - 1)
            {
                t_point3d point_right = {x + 1, y, map->z_matrix[y][x + 1]};
                project_iso(point_right, &projected_end, vars);
                draw_line_between_points(img, point_offset(projected_start, *offset), point_offset(projected_end, *offset), map->color_matrix[y][x]);
            }

            if (y < map->height - 1)
            {
                t_point3d point_down = {x, y + 1, map->z_matrix[y + 1][x]};
                project_iso(point_down, &projected_end, vars);
                draw_line_between_points(img, point_offset(projected_start, *offset), point_offset(projected_end, *offset), map->color_matrix[y][x]);
            }
        }
    }
}

void    render_screen(t_vars *vars)
{
    t_point2d center_offset;
    t_point2d img_size;
    t_point2d offset;
    
     // Redraw the map with updated parameters
    mlx_clear_window(vars->mlx, vars->win);
    if (vars->img.img)
        mlx_destroy_image(vars->mlx, vars->img.img);
    img_size = calculate_img(vars->map, vars, &offset);
    vars->img.img = mlx_new_image(vars->mlx, img_size.x + 2, img_size.y + 2);
    vars->img.addr = mlx_get_data_addr(vars->img.img, &vars->img.bits_per_pixel, &vars->img.line_length, &vars->img.endian);
    printf("img_size.x: %d, img_size.y: %d\n", img_size.x, img_size.y);
    // print offset
    printf("offset.x: %d, offset.y: %d\n", offset.x, offset.y);
    draw_map(&vars->img, vars->map, vars, &offset); // Use vars->map
    center_offset = rect_offset_center((t_point2d){WINDOW_WIDTH, WINDOW_HEIGHT}, img_size);
    mlx_put_image_to_window(vars->mlx, vars->win, vars->img.img, center_offset.x + vars->shift_x, center_offset.y + vars->shift_y);
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
    printf("key: %d", keycode);
    if (keycode == KEY_ESC)
    {
        free_vars(vars);
        exit(0);
    }
    else if (keycode == KEY_PLUS) // ist falsch ist
        vars->zoom += 1;
    else if (keycode == KEY_MINUS)
        vars->zoom -= 1;
    else if (keycode == KEY_LEFT)
        vars->shift_x -= 10;
    else if (keycode == KEY_RIGHT)
        vars->shift_x += 10;
    else if (keycode == KEY_UP)
        vars->shift_y -= 10;
    else if (keycode == KEY_DOWN)
        vars->shift_y += 10;
    else if (keycode == 113) // 'q' key to rotate left
        vars->angle -= 0.05;
    else if (keycode == 101) // 'e' key to rotate right
        vars->angle += 0.05;
    else if (keycode == 97) // 'a' key to decrease z-scale
        vars->z_scale += 0.1;
    else if (keycode == 100) // 'd' key to increase z-scale
        vars->z_scale -= 0.1;

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
    vars.zoom = 20; // Set initial zoom level
    vars.angle = 0.8; // Approximately 45 degrees in radians
    vars.z_scale = 1.0;
    vars.shift_x = 0; // Center the map
    vars.shift_y = 0;
    vars.map = &map; // Store map in vars for access in event handlers

    render_screen(&vars);

    mlx_key_hook(vars.win, handle_key, &vars);
    mlx_mouse_hook(vars.win, handle_mouse, &vars);
    mlx_hook(vars.win, 17, 1L << 17, close_window, &vars);
    mlx_loop(vars.mlx);

    free_map(&map);
    return (0);
}
