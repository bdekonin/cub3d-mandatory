/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   parse_main.c                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: bdekonin <bdekonin@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/02/19 14:26:42 by bdekonin      #+#    #+#                 */
/*   Updated: 2020/04/10 13:18:12 by bdekonin      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "parse_data.h"
#include "../cub3d.h"

static void		copy_dir(t_vars *vars, t_data *data)
{
	if (data->spawn_dir == 'N')
	{
		vars->player.dir_y = -1;
		vars->cam.planeX = 0.66;
		vars->cam.planeY = 0;
	}
	else if (data->spawn_dir == 'E')
	{
		vars->player.dir_x = 1;
		vars->player.dir_y = 0;
		vars->cam.planeY = 0.66;
	}
	else if (data->spawn_dir == 'S')
	{
		vars->player.dir_y = 1;
		vars->cam.planeX = -0.66;
		vars->cam.planeY = 0;
	}
	else if (data->spawn_dir == 'W')
	{
		vars->player.dir_x = -1;
		vars->player.dir_y = 0;
		vars->cam.planeY = -0.66;
	}
}

static void		copy_data(t_vars *vars, t_data *data)
{
	vars->screen.screen_w = data->screen_x;
	vars->screen.screen_h = data->screen_y;
	vars->player.pos_x = data->spawn_pos_x;
	vars->player.pos_y = data->spawn_pos_y;
	vars->cam.planeX = 0;
	vars->player.dir_x = 0;
	copy_dir(vars, data);
	vars->map.map = data->map;
	vars->map.map_w = data->map_width;
	vars->map.map_h = data->map_height;
	vars->cub.floor_color = data->floor[0] * 65536 +
	data->floor[1] * 256 + data->floor[2];
	vars->cub.ceiling_color = data->ceiling[0] * 65536 +
	data->ceiling[1] * 256 + data->ceiling[2];
	vars->cub.north = data->north_path;
	vars->cub.east = data->east_path;
	vars->cub.south = data->south_path;
	vars->cub.west = data->west_path;
	vars->cub.sprite = data->sprite_path;
	vars->spr.sprite = data->sprite;
	vars->spr.sprite_count = data->sprite_count;
}

static int		line_select(t_data *data, int i)
{
	if (data->line[i] == 'R')
		return (get_resolution(data->line + 1, data));
	else if (data->line[i] == 'N' && data->line[i + 1] == 'O')
		return (get_north(data->line + i + 2, data));
	else if (data->line[i] == 'E' && data->line[i + 1] == 'A')
		return (get_east(data->line + i + 2, data));
	else if (data->line[i] == 'S' && data->line[i + 1] == 'O')
		return (get_south(data->line + i + 2, data));
	else if (data->line[i] == 'S' && data->line[i + 1] == ' ')
		return (get_sprite(data->line + i + 1, data));
	else if (data->line[i] == 'W' && data->line[i + 1] == 'E')
		return (get_west(data->line + i + 2, data));
	else if (data->line[i] == 'C')
		return (get_ceiling(data->line + i, data));
	else if (data->line[i] == 'F')
		return (get_floor(data->line + i, data));
	else if (data->line[i] == '0' || data->line[i] == '1' || data->line[i] == '2')
	{
		read_map(data->line, &data->map_width, &data->map_height, &data->sprite_count);
		if (data->map_start == -1 && (ft_counter(data->line, '1') \
									+ ft_counter(data->line, '0') > 0))
			data->map_start = data->count;
	}
	else
		return (ft_puterror("Invalid line."));
	return (1);
}

static int		parse_error_check(t_data *data, char *argv)
{
	close(data->fd);
	if (data->map_start == -1)
	{
		ft_puterror("map missing.\n");
		return (parse_free(data));
	}
	if (copy_map(data, argv) == -1)
		return (parse_free(data));
	if (!check_path(data, data->spawn_pos_y, data->spawn_pos_x, 0))
	{
		ft_puterror("flood fill failed\n");
		return (parse_free(data));
	}
	transfer_map(data);
	if (!missing_elem(data))
	{
		ft_puterror("element missing.\n");
		return (parse_free(data));
	}
	return (1);
}

static int		parse_loop(t_data *data)
{
	int i;

	while (data->ret > 0 && data->error != -1)
	{
		i = 0;
		data->ret = get_next_line(data->fd, &data->line);
		if (data->ret < 0)
		{
			parse_free(data);
			return (ft_puterror("parse_main | get_next_line failed."));
		}
		while (data->line[i] == ' ' && data->line[i] != '\0')
			i++;
		if (data->line[i] == '\0' && i != 0)
		{
			ft_puterror("Invalid line");
			return (parse_free(data));
		}
		if (data->line[i] != '\0' && line_select(data, i) == -1)
			data->error = -1;
		free(data->line);
		data->count++;
		if (data->error == -1)
			return (parse_free(data));
	}
	return (1);
}

int				parse_main(t_vars *vars, char *argv)
{
	t_data		data;

	data.fd = open(argv, O_RDONLY);
	if (data.fd < 0)
		return (ft_puterror("Argument not found."));
	parse_init(&data);
	if (parse_loop(&data) == -1)
		return (-1);
	if (parse_error_check(&data, argv) == -1)
		return (-1);
	copy_data(vars, &data);
	return (data.error);
}
