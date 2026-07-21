/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cycle.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-azim <yel-azim@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 17:54:59 by yel-azim          #+#    #+#             */
/*   Updated: 2026/05/09 17:55:01 by yel-azim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "codexion.h"

static void	dongle_order(
	t_coder *coder,
	t_dongle **first,
	t_dongle **second
)
{
	if (coder->coder_id % 2 == 0)
	{
		*first = coder->left_dongle;
		*second = coder->right_dongle;
		return ;
	}
	*first = coder->right_dongle;
	*second = coder->left_dongle;
}

static void	take_dongles(t_coder *coder)
{
	t_simulation	*sim;
	t_dongle		*first;
	t_dongle		*second;

	sim = coder->simulation;
	dongle_order(coder, &first, &second);
	add_to_queue(coder, first);
	add_to_queue(coder, second);
	wait_dongle(coder, first);
	if (get_is_over(sim))
		return (remove_from_queue(coder, second));
	print_status(sim, coder->coder_id, "has taken a dongle");
	wait_dongle(coder, second);
	if (get_is_over(sim))
	{
		release_dongle(first);
		remove_from_queue(coder, second);
		return ;
	}
	print_status(sim, coder->coder_id, "has taken a dongle");
}

static void	compile_cycle(t_coder *coder)
{
	t_simulation	*sim;

	sim = coder->simulation;
	if (get_is_over(sim))
		return (release_dongle(coder->left_dongle),
			release_dongle(coder->right_dongle));
	pthread_mutex_lock(&coder->coder_mutex);
	if (coder->compile_counter >= sim->required_compiles)
		return (pthread_mutex_unlock(&coder->coder_mutex),
			release_dongle(coder->left_dongle),
			release_dongle(coder->right_dongle));
	pthread_mutex_unlock(&coder->coder_mutex);
	print_status(sim, coder->coder_id, "is compiling");
	pthread_mutex_lock(&coder->coder_mutex);
	coder->last_compile_time = get_current_time();
	pthread_mutex_unlock(&coder->coder_mutex);
	safe_sleep(coder, sim->time_to_compile);
	pthread_mutex_lock(&coder->coder_mutex);
	coder->compile_counter++;
	pthread_mutex_unlock(&coder->coder_mutex);
	remove_from_queue(coder, coder->left_dongle);
	remove_from_queue(coder, coder->right_dongle);
	release_dongle(coder->left_dongle);
	release_dongle(coder->right_dongle);
}

static void	rest_cycle(t_coder *coder)
{
	t_simulation	*sim;

	sim = coder->simulation;
	if (get_is_over(sim))
		return ;
	print_status(sim, coder->coder_id, "is debugging");
	safe_sleep(coder, sim->time_to_debug);
	if (get_is_over(sim))
		return ;
	print_status(sim, coder->coder_id, "is refactoring");
	safe_sleep(coder, sim->time_to_refactor);
}

void	*simulation_cycle(void *arg)
{
	int				count;
	t_coder			*coder;
	t_simulation	*sim;

	coder = (t_coder *)arg;
	sim = coder->simulation;
	if (sim->total_coders == 1)
		return (single_coder_cycle(coder));
	// if (coder->coder_id % 2 == 0)
	// 	usleep(100);
	while (!get_is_over(sim))
	{
		pthread_mutex_lock(&coder->coder_mutex);
		count = coder->compile_counter;
		pthread_mutex_unlock(&coder->coder_mutex);
		if (count >= sim->required_compiles)
			return (NULL);
		take_dongles(coder);
		if (get_is_over(sim))
			break ;
		compile_cycle(coder);
		rest_cycle(coder);
	}
	return (NULL);
}
