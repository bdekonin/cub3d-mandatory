/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.c                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: bdekonin <bdekonin@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/02/12 13:41:15 by bdekonin       #+#    #+#                */
/*   Updated: 2020/03/10 16:42:39 by bdekonin      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "parsing/parse_data.h"
#include "cub3d.h"
#include <math.h>
#include <stdio.h>

int initialize_rendering(t_vars *vars);
int engine(t_vars *vars);

int create_img(t_vars *vars, char *filename)
{
	char *ptr;
	ptr = ft_strjoin("cub3d | ", filename);
	if (!ptr)
		return (ft_puterror("Malloc failed."));
	vars->mlx.mlx = mlx_init();
	if (vars->save == 0)
		vars->mlx.mlx_win = mlx_new_window(vars->mlx.mlx, vars->screen.screen_w, vars->screen.screen_h, ptr);
    vars->mlx.img = mlx_new_image(vars->mlx.mlx, vars->screen.screen_w, vars->screen.screen_h);
    vars->mlx.addr = mlx_get_data_addr(vars->mlx.img, &vars->mlx.bits_pixel, &vars->mlx.line_length,
                                 &vars->mlx.endian);

    vars->nframe.img = mlx_new_image(vars->mlx.mlx, vars->screen.screen_w, vars->screen.screen_h);
    vars->nframe.addr = mlx_get_data_addr(vars->nframe.img, &vars->nframe.bits_pixel, &vars->nframe.line_length,
                                 &vars->nframe.endian);
	free(ptr);
	if (file_north(vars) == -1)
		return (-1);
	if (file_east(vars) == -1)
		return (-1);
	if (file_south(vars) == -1)
		return (-1);
	if (file_west(vars) == -1)
		return (-1);
	if (file_sprite(vars) == -1)
		return (-1);
	return (1);
}

void sortsprite_one(t_vars *vars)
{
	for (int i = 0; i < vars->spr.sprite_count - 1; i++)
	{
		for (int i = 0; i < vars->spr.sprite_count - 1; i++)
		{
			if (vars->spr.sprite[i][2] < vars->spr.sprite[i + 1][2])
			{
				swap(&vars->spr.sprite[i][0], &vars->spr.sprite[i + 1][0]);
				swap(&vars->spr.sprite[i][1], &vars->spr.sprite[i + 1][1]);
				swap(&vars->spr.sprite[i][2], &vars->spr.sprite[i + 1][2]);
			}
		}
	}
}

void calculate_dist_one(t_vars *vars)
{
	int i;
	int y;
	int x;

	i = 0;
	while (i < vars->spr.sprite_count)
	{
		x = vars->player.pos_x - vars->spr.sprite[i][1];
		x *= (x < 0) ? -1 : 1;
		y = vars->player.pos_y - vars->spr.sprite[i][0];
		y *= (y < 0) ? -1 : 1;
		vars->spr.sprite[i][2] = sqrt(y + x);
		i++;
	}
	sortsprite_one(vars);
}

int	main(int argc, char **argv)
{
	t_vars		vars;
	int			ret;
	char		*p;

	if (argc < 2)
		return (ft_puterror("Not enough arguments."));
	if (argc > 3)
		return (ft_puterror("Too many arguments."));
	p = ft_strrchr(argv[1], '.');
	if (!p || ft_strncmp(p, ".cub", 10) != 0)
		return (ft_puterror("Argument is not a .cub file."));
	if (argc > 2 && ft_strncmp(argv[2], "--save", 7))
		return (ft_puterror("Second argument is invalid."));
	else if (argc > 2 && ft_strncmp(argv[2], "--save", 7) == 0)
		vars.save = 1;
	ret = parse_main(&vars, argv[1]);
	if (ret == -1)
		exit(1);
	initialize_rendering(&vars);
	if (create_img(&vars, argv[1]) == -1)
		exit(1);
	if (vars.save == 1)
		createbmp(&vars);
	else
		engine(&vars);
	system("leaks cub3D");
	exit(0);
}

void renderframe(t_vars *vars)
{
	int texWidth = 64;
	int	texHeight = 64;
	double ZBuffer[vars->screen.screen_w];
	
	for(int x = 0; x < vars->screen.screen_w; x++)
	{
		vars->cam.camera_x = 2 * x / (double)vars->screen.screen_w - 1;
		vars->eng.raydir_x = vars->player.dir_x + vars->cam.planeX * vars->cam.camera_x;
		vars->eng.raydir_y = vars->player.dir_y + vars->cam.planeY * vars->cam.camera_x;
		vars->map.pos_x = (int)vars->player.pos_x;
		vars->map.pos_y = (int)vars->player.pos_y;

		vars->eng.delta_dist_x = fabs(1 / vars->eng.raydir_x);
		vars->eng.delta_dist_y = fabs(1 / vars->eng.raydir_y);

		vars->eng.hit = 0;

		if (vars->eng.raydir_x < 0)
		{
			vars->eng.step_x = -1;
			vars->eng.side_dist_x = (vars->player.pos_x - vars->map.pos_x) * vars->eng.delta_dist_x;
		}
		else
		{
			vars->eng.step_x = 1;
			vars->eng.side_dist_x = (vars->map.pos_x + 1.0 - vars->player.pos_x) * vars->eng.delta_dist_x;
		}
		if (vars->eng.raydir_y < 0)
		{
			vars->eng.step_y = -1;
			vars->eng.side_dist_y = (vars->player.pos_y - vars->map.pos_y) * vars->eng.delta_dist_y;
		}
		else
		{
			vars->eng.step_y = 1;
			vars->eng.side_dist_y = (vars->map.pos_y + 1.0 - vars->player.pos_y) * vars->eng.delta_dist_y;
		}
		while (vars->eng.hit == 0)
		{
			if (vars->eng.side_dist_x < vars->eng.side_dist_y)
			{
				vars->eng.side_dist_x += vars->eng.delta_dist_x;
				vars->map.pos_x += vars->eng.step_x;
				vars->eng.side = 0;
			}
			else
			{
				vars->eng.side_dist_y += vars->eng.delta_dist_y;
				vars->map.pos_y += vars->eng.step_y;
				vars->eng.side = 1;
			}
			if (vars->map.map[vars->map.pos_y][vars->map.pos_x] > 0 && vars->map.map[vars->map.pos_y][vars->map.pos_x] != 2)
				vars->eng.hit = 1;
		}
		wallsides(vars);
			if (vars->tex.w_tex == 'N')
			{
				texWidth = vars->tex.w[0];
				texHeight = vars->tex.h[0];
			}
			else if (vars->tex.w_tex == 'E')
			{
				texWidth = vars->tex.w[1];
				texHeight = vars->tex.h[1];
			}
			else if (vars->tex.w_tex == 'S')
			{
				texWidth = vars->tex.w[2];
				texHeight = vars->tex.h[2];
			}
			else if (vars->tex.w_tex == 'W')
			{
				texWidth = vars->tex.w[3];
				texHeight = vars->tex.h[3];
			}

		// calcuate line height
		int lineHeight = (int)vars->screen.screen_h / vars->eng.perp_wall_dist;

		//calculate lowest heighest pixel
		int drawStart = -lineHeight / 2 + vars->screen.screen_h / 2;
		if (drawStart < 0)
			drawStart = 0;
		int drawEnd = lineHeight / 2 + vars->screen.screen_h / 2; 
		if (drawEnd >= vars->screen.screen_h)
			drawEnd = vars->screen.screen_h - 1;

		//texturing calculations
		int texNum = vars->map.map[vars->map.pos_y][vars->map.pos_x]; //1 subtracted from it so that texture 0 can be used!
			double wallX;
			if (vars->eng.side == 0)
				wallX = vars->player.pos_y + vars->eng.perp_wall_dist * vars->eng.raydir_y;
			else
				wallX = vars->player.pos_x + vars->eng.perp_wall_dist * vars->eng.raydir_x;
			wallX -= floor((wallX));

		int texX = (int)(wallX * (double)(texWidth));
		if (vars->eng.side == 0 && vars->eng.raydir_x > 0)
			texX = texWidth - texX - 1;
		if (vars->eng.side == 1 && vars->eng.raydir_y < 0)
			texX = texWidth - texX - 1;


		// How much to increase the texture coordinate per screen pixel
    	double step = 1.0 * texHeight / lineHeight;
		double texPos = (drawStart - vars->screen.screen_h / 2 + lineHeight / 2) * step;

		if (vars->map.map[(int)vars->player.pos_y][(int)vars->player.pos_x] == 3)
			vars->map.map[(int)vars->player.pos_y][(int)vars->player.pos_x] = 0;
			
		for(int y = drawStart; y<drawEnd; y++)
		{
			if (texNum != 2)
			{
				unsigned int color;
				int texY = (int)texPos & (texHeight - 1);
				texPos += step;
				if (vars->tex.w_tex == 'N')
					color = *(unsigned int*)(vars->tex.addr[0] + (texY * vars->tex.line_length[0] + texX * (vars->tex.bits_pixel[0] / 8)));
				else if (vars->tex.w_tex == 'E')
					color = *(unsigned int*)(vars->tex.addr[1] + (texY * vars->tex.line_length[1] + texX * (vars->tex.bits_pixel[1] / 8)));
				else if (vars->tex.w_tex == 'S')
					color = *(unsigned int*)(vars->tex.addr[2] + (texY * vars->tex.line_length[2] + texX * (vars->tex.bits_pixel[2] / 8)));
				else if (vars->tex.w_tex == 'W')
					color = *(unsigned int*)(vars->tex.addr[3] + (texY * vars->tex.line_length[3] + texX * (vars->tex.bits_pixel[3] / 8)));
				my_mlx_pixel_put(vars, x, y, color);
			}
		}
		ZBuffer[x] = vars->eng.perp_wall_dist;
		fill_background(x, drawStart, drawEnd, vars);
	}
	for(int i = 0; i < vars->spr.sprite_count; i++)
	{
		double spriteY = vars->spr.sprite[i][0] - vars->player.pos_y;
		double spriteX = vars->spr.sprite[i][1] - vars->player.pos_x;

		double invDet = 1.0 / (vars->cam.planeX * vars->player.dir_y - vars->player.dir_x * vars->cam.planeY);
		double transformX = invDet * (vars->player.dir_y * spriteX - vars->player.dir_x * spriteY);
		double transformY = invDet * (-vars->cam.planeY * spriteX + vars->cam.planeX * spriteY);

		int spriteScreenX = (int)(vars->screen.screen_w / 2) * (1 + transformX / transformY);
		
		int spriteHeight = fabs((int)vars->screen.screen_h / transformY);

		int drawStartY = -spriteHeight / 2 + vars->screen.screen_h / 2;
		if (drawStartY < 0)
			drawStartY = 0;
		int drawEndY = spriteHeight / 2 + vars->screen.screen_h / 2;
		if (drawEndY >= vars->screen.screen_h)
			drawEndY = vars->screen.screen_h - 1;

		int spriteWidth = fabs((int)vars->screen.screen_h / transformY);
		int drawStartX = -spriteWidth / 2 + spriteScreenX;
		if (drawStartX < 0)
			drawStartX = 0;
		int drawEndX = spriteWidth / 2 + spriteScreenX;
		if (drawEndX >= vars->screen.screen_w)
			drawEndX = vars->screen.screen_w - 1;
		
		for(int stripe = drawStartX; stripe < drawEndX; stripe++)
		{
			unsigned int color;
			int texX = (256 *(stripe - (-spriteWidth / 2 + spriteScreenX)) * vars->tex.w[4] / spriteWidth) / 256;
			if (transformY > 0 && stripe > 0 && stripe < vars->screen.screen_w && transformY < ZBuffer[stripe])
			for(int y = drawStartY; y < drawEndY; y++)
			{
				int d = y * 256 - vars->screen.screen_h * 128 + spriteHeight * 128;
				int texY = (d * vars->tex.h[4]) / spriteHeight / 256;
				color = *(unsigned int*)(vars->tex.addr[4] + (texY * vars->tex.line_length[4] + texX * (vars->tex.bits_pixel[4] / 8)));
				if ((color & 0x00FFFFFF) != 0)
					my_mlx_pixel_put(vars, stripe, y, color);
			}
		}
	}
	calculate_dist_one(vars);
}

int testing(t_vars *vars)
{
	renderframe(vars);
	if (vars->key.esc == 1)
		close_win(vars);
	if (vars->key.w == 1)
		forward(vars);
	else if (vars->key.s == 1)
		backwards(vars);
	if (vars->key.l_arr == 1)
		look_left(vars);
	else if (vars->key.r_arr == 1)
		look_right(vars);
	if (vars->key.a == 1)
		walk_left(vars);
	else if (vars->key.d == 1)
		walk_right(vars);
	if (vars->image == 0)
	{
		mlx_put_image_to_window(vars->mlx.mlx, vars->mlx.mlx_win, vars->mlx.img, 0, 0);
		vars->image = 1;
	}
	else
	{
		mlx_put_image_to_window(vars->mlx.mlx, vars->mlx.mlx_win, vars->nframe.img, 0, 0);
		vars->image = 0;
	}
	return (1);
}

int initialize_rendering(t_vars *vars)
{
	int x;
	int y;
	init_key(vars);
	vars->cam.rot_speed = 0.05;
	vars->cam.move_speed = 0.09;
	vars->player.pos_x += 0.5;
	vars->player.pos_y += 0.5;
	if (vars->save == 0)
	{
		mlx_get_screen_size(vars->mlx.mlx, &x, &y);
		vars->screen.screen_h = (vars->screen.screen_h > y) ? y : vars->screen.screen_h;
		vars->screen.screen_w = (vars->screen.screen_w > x) ? x : vars->screen.screen_w;
	}
	return (1);
}

int engine(t_vars *vars)
{
	mlx_loop_hook(vars->mlx.mlx, testing, vars);
	mlx_hook(vars->mlx.mlx_win, 2, (1L << 0), key_press, vars);
	mlx_hook(vars->mlx.mlx_win, 3, (1L << 1), key_release, vars);
	mlx_hook(vars->mlx.mlx_win, 17, 0L, close_win, vars);
	mlx_loop(vars->mlx.mlx);
	return (1);
}
