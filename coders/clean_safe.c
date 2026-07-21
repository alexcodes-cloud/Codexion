/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clean_safe.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-azim <yel-azim@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 01:37:25 by yel-azim          #+#    #+#             */
/*   Updated: 2026/06/19 18:30:01 by yel-azim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "codexion.h"

void	print_status(t_simulation *sim, int coder_id, char *text)
{
	pthread_mutex_lock(&sim->write_lock);
	if (!get_is_over(sim) || strcmp(text, "burned out") == 0)
		printf("%ld %d %s\n", get_elapsed_time(sim), coder_id, text);
	pthread_mutex_unlock(&sim->write_lock);
}

void	cleanup_simulation(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->total_coders)
	{
		pthread_join(sim->coders[i].coder_thread, NULL);
		i++;
	}
	i = 0;
	while (i < sim->total_coders)
	{
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		pthread_cond_destroy(&sim->dongles[i].cond);
		pthread_mutex_destroy(&sim->coders[i].coder_mutex);
		i++;
	}
	pthread_mutex_destroy(&sim->write_lock);
	pthread_mutex_destroy(&sim->is_over_mutex);
	free(sim->coders);
	free(sim->dongles);
}
