/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-azim <yel-azim@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/18 16:02:51 by yel-azim          #+#    #+#             */
/*   Updated: 2026/05/18 16:02:53 by yel-azim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "codexion.h"

static void	sort_slots(t_dongle *dongle, char *scheduler)
{
	t_queue	tmp;

	if (dongle->slot_count < 2)
		return ;
	if (strcmp(scheduler, "fifo") == 0)
	{
		if (dongle->slots[1].arrival_time < dongle->slots[0].arrival_time)
		{
			tmp = dongle->slots[0];
			dongle->slots[0] = dongle->slots[1];
			dongle->slots[1] = tmp;
		}
	}
	else
	{
		if (dongle->slots[1].deadline < dongle->slots[0].deadline)
		{
			tmp = dongle->slots[0];
			dongle->slots[0] = dongle->slots[1];
			dongle->slots[1] = tmp;
		}
	}
}

void	add_to_queue(t_coder *coder, t_dongle *dongle)
{
	t_simulation	*sim;
	long			last_compile;

	sim = coder->simulation;
	pthread_mutex_lock(&coder->coder_mutex);
	last_compile = coder->last_compile_time;
	pthread_mutex_unlock(&coder->coder_mutex);
	pthread_mutex_lock(&dongle->mutex);
	if (dongle->slot_count < 2)
	{
		dongle->slots[dongle->slot_count].coder = coder;
		dongle->slots[dongle->slot_count].deadline = last_compile
			+ sim->time_to_burnout;
		dongle->slots[dongle->slot_count].arrival_time = get_current_time();
		dongle->slot_count++;
		sort_slots(dongle, sim->scheduler);
	}
	pthread_mutex_unlock(&dongle->mutex);
}

void	remove_from_queue(t_coder *coder, t_dongle *dongle)
{
	int	i;

	pthread_mutex_lock(&dongle->mutex);
	i = 0;
	while (i < dongle->slot_count)
	{
		if (dongle->slots[i].coder == coder)
		{
			if (i == 0 && dongle->slot_count == 2)
				dongle->slots[0] = dongle->slots[1];
			dongle->slot_count--;
			break ;
		}
		i++;
	}
	pthread_mutex_unlock(&dongle->mutex);
}

void	wait_dongle(t_coder *coder, t_dongle *dongle)
{
	t_simulation	*sim;
	long			wait_ms;

	sim = coder->simulation;
	pthread_mutex_lock(&dongle->mutex);
	while (!get_is_over(sim))
	{
		if (!dongle->in_use && dongle->slot_count > 0
			&& dongle->slots[0].coder == coder)
			break ;
		if (!dongle->in_use && dongle->slot_count == 0)
			break ;
		pthread_cond_wait(&dongle->cond, &dongle->mutex);
	}
	pthread_mutex_unlock(&dongle->mutex);
	if (get_is_over(sim))
		return ;
	wait_ms = sim->dongle_cooldown
		- (get_current_time() - dongle->last_release_time);
	if (wait_ms > 0)
		usleep(wait_ms * 1000);
	pthread_mutex_lock(&dongle->mutex);
	dongle->in_use = 1;
	pthread_mutex_unlock(&dongle->mutex);
}

void	release_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->in_use = 0;
	dongle->last_release_time = get_current_time();
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}
