/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-azim <yel-azim@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 17:55:05 by yel-azim          #+#    #+#             */
/*   Updated: 2026/05/09 17:55:06 by yel-azim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "codexion.h"

long	get_current_time(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) == 0)
		return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
	return (-1);
}

long	get_elapsed_time(t_simulation *sim)
{
	long	current;

	current = get_current_time();
	if (current == -1)
		return (-1);
	return (current - sim->start_time);
}

int	get_is_over(t_simulation *sim)
{
	int	value;

	pthread_mutex_lock(&sim->is_over_mutex);
	value = sim->is_over;
	pthread_mutex_unlock(&sim->is_over_mutex);
	return (value);
}

void	set_is_over(t_simulation *sim, int value)
{
	pthread_mutex_lock(&sim->is_over_mutex);
	sim->is_over = value;
	pthread_mutex_unlock(&sim->is_over_mutex);
}

void	safe_sleep(t_coder *coder, long ms)
{
	long	start;

	start = get_current_time();
	while (!get_is_over(coder->simulation))
	{
		if (get_current_time() - start >= ms)
			break ;
		usleep(500);
	}
}
