/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-azim <yel-azim@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 15:39:56 by yel-azim          #+#    #+#             */
/*   Updated: 2026/05/11 15:39:59 by yel-azim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "codexion.h"

static void	burned_out_message(t_simulation *sim, int coder_id)
{
	pthread_mutex_lock(&sim->write_lock);
	printf("%ld %d burned out\n", get_elapsed_time(sim), coder_id);
	pthread_mutex_unlock(&sim->write_lock);
}

static void	wake_all_waiters(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->total_coders)
	{
		pthread_mutex_lock(&sim->dongles[i].mutex);
		pthread_cond_broadcast(&sim->dongles[i].cond);
		pthread_mutex_unlock(&sim->dongles[i].mutex);
		i++;
	}
}

static int	all_finished(t_simulation *sim)
{
	int	i;
	int	count;

	i = 0;
	while (i < sim->total_coders)
	{
		pthread_mutex_lock(&sim->coders[i].coder_mutex);
		count = sim->coders[i].compile_counter;
		pthread_mutex_unlock(&sim->coders[i].coder_mutex);
		if (count < sim->required_compiles)
			return (0);
		i++;
	}
	return (1);
}

static int	check_burnout(t_simulation *sim)
{
	int		i;
	long	now;
	long	last_compile;
	int		count;

	now = get_current_time();
	i = 0;
	while (i < sim->total_coders)
	{
		pthread_mutex_lock(&sim->coders[i].coder_mutex);
		last_compile = sim->coders[i].last_compile_time;
		count = sim->coders[i].compile_counter;
		pthread_mutex_unlock(&sim->coders[i].coder_mutex);
		if (count < sim->required_compiles)
			if (now - last_compile > sim->time_to_burnout)
				return (set_is_over(sim, 1),
					wake_all_waiters(sim),
					burned_out_message(sim, sim->coders[i].coder_id), 1);
		i++;
	}
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_simulation	*sim;

	sim = (t_simulation *)arg;
	while (1)
	{
		if (all_finished(sim))
		{
			set_is_over(sim, 1);
			wake_all_waiters(sim);
			return (NULL);
		}
		if (check_burnout(sim))
			return (NULL);
		usleep(10);
	}
	return (NULL);
}
