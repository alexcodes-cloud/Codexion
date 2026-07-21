/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-azim <yel-azim@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 17:54:53 by yel-azim          #+#    #+#             */
/*   Updated: 2026/05/09 17:54:55 by yel-azim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "codexion.h"

int	main(int ac, char *av[])
{
	t_simulation	sim;
	pthread_t		monitor_thread;

	if (ac != 9)
	{
		printf("Usage: %s number_of_coders time_to_burnout time_to_compile"
			" time_to_debug time_to_refactor number_of_compiles_required"
			" dongle_cooldown scheduler(fifo/edf)\n", av[0]);
		return (1);
	}
	if (!parse_data(av))
		return (1);
	if (!setup_simulation(&sim, av))
	{
		printf("Error: setup failed\n");
		return (1);
	}
	create_threads(&sim, &monitor_thread);
	cleanup_simulation(&sim);
	return (0);
}
