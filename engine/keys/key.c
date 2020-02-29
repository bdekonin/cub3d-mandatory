/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   key.c                                              :+:    :+:            */
/*                                                     +:+                    */
/*   By: bdekonin <bdekonin@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/02/21 14:31:59 by bdekonin       #+#    #+#                */
/*   Updated: 2020/02/23 09:38:42 by bdekonin      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../cub3d.h"

int		close_win(t_vars *vars)
{
	mlx_destroy_window(vars->mlx.mlx, vars->mlx.mlx_win);
	exit(0);
	return (1);
}

int		key_press(int keycode, t_vars *vars)
{
	if (keycode == 53)
		vars->key.esc = 1;
	else if (keycode == 13)
		vars->key.w = 1;
	else if (keycode == 0)
		vars->key.a = 1;
	else if (keycode == 1)
		vars->key.s = 1;
	else if (keycode == 2)
		vars->key.d = 1;
	else if (keycode == 123)
		vars->key.l_arr = 1;
	else if (keycode == 124)
		vars->key.r_arr = 1;
	else if (keycode == 49)
		vars->key.space = 1;
	else if (keycode == 45)
		vars->key.n = 1;
	return (1);
}

int		key_release(int keycode, t_vars *vars)
{
	if (keycode == 53)
		vars->key.esc = 0;
	else if (keycode == 13)
		vars->key.w = 0;
	else if (keycode == 0)
		vars->key.a = 0;
	else if (keycode == 1)
		vars->key.s = 0;
	else if (keycode == 2)
		vars->key.d = 0;
	else if (keycode == 123)
		vars->key.l_arr = 0;
	else if (keycode == 124)
		vars->key.r_arr = 0;
	else if (keycode == 49)
		vars->key.space = 0;
	else if (keycode == 45)
		vars->key.n = 0;
	return (1);
}

void	init_key(t_vars *vars)
{
	vars->key.esc = 0;
	vars->key.w = 0;
	vars->key.a = 0;
	vars->key.s = 0;
	vars->key.d = 0;
	vars->key.l_arr = 0;
	vars->key.r_arr = 0;
	vars->key.space = 0;
	vars->key.n = 0;
}
