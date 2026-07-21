/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-azim <yel-azim@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 17:55:11 by yel-azim          #+#    #+#             */
/*   Updated: 2026/05/09 17:55:13 by yel-azim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "codexion.h"

static int	init_simulation_data(t_simulation *sim, char **av)
{
	sim->total_coders = atoi(av[1]);
	sim->time_to_burnout = atoi(av[2]);
	sim->time_to_compile = atoi(av[3]);
	sim->time_to_debug = atoi(av[4]);
	sim->time_to_refactor = atoi(av[5]);
	sim->required_compiles = atoi(av[6]);
	sim->dongle_cooldown = atoi(av[7]);
	sim->scheduler = av[8];
	sim->is_over = 0;
	sim->start_time = get_current_time();
	sim->coders = malloc(sizeof(t_coder) * sim->total_coders);
	sim->dongles = malloc(sizeof(t_dongle) * sim->total_coders);
	if (!sim->coders || !sim->dongles)
		return (0);
	return (1);
}

static void	init_dongle(t_simulation *sim, int i)
{
	sim->dongles[i].dongle_id = i + 1;
	sim->dongles[i].in_use = 0;
	sim->dongles[i].last_release_time = 0;
	sim->dongles[i].slot_count = 0;
	pthread_mutex_init(&sim->dongles[i].mutex, NULL);
	pthread_cond_init(&sim->dongles[i].cond, NULL);
}

static void	init_coder(t_simulation *sim, int i)
{
	sim->coders[i].coder_id = i + 1;
	sim->coders[i].simulation = sim;
	sim->coders[i].left_dongle = &sim->dongles[i];
	sim->coders[i].right_dongle = &sim->dongles[(i + 1) % sim->total_coders];
	sim->coders[i].last_compile_time = sim->start_time;
	sim->coders[i].compile_counter = 0;
	pthread_mutex_init(&sim->coders[i].coder_mutex, NULL);
}

int	setup_simulation(t_simulation *sim, char **av)
{
	int	i;

	if (!init_simulation_data(sim, av))
		return (0);
	pthread_mutex_init(&sim->write_lock, NULL);
	pthread_mutex_init(&sim->is_over_mutex, NULL);
	i = 0;
	while (i < sim->total_coders)
	{
		init_dongle(sim, i);
		i++;
	}
	i = 0;
	while (i < sim->total_coders)
	{
		init_coder(sim, i);
		i++;
	}
	return (1);
}

void	create_threads(t_simulation *sim, pthread_t *monitor_thread)
{
	int	i;

	i = 0;
	while (i < sim->total_coders)
	{
		pthread_create(&sim->coders[i].coder_thread, NULL,
			&simulation_cycle, &sim->coders[i]);
		i++;
	}
	pthread_create(monitor_thread, NULL, &monitor_routine, sim);
	pthread_join(*monitor_thread, NULL);
}
