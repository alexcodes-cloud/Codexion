/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-azim <yel-azim@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 12:46:02 by yel-azim          #+#    #+#             */
/*   Updated: 2026/05/09 12:46:09 by yel-azim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include <pthread.h>
# include <string.h>
# include <sys/time.h>

typedef struct s_simulation	t_simulation;

typedef struct s_queue
{
	struct s_coder	*coder;
	long			deadline;
	long			arrival_time;
}	t_queue;

typedef struct s_dongle
{
	int				dongle_id;
	int				in_use;
	long			last_release_time;
	t_queue			slots[2];
	int				slot_count;
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
}	t_dongle;

typedef struct s_coder
{
	int				coder_id;
	pthread_t		coder_thread;
	t_dongle		*left_dongle;
	t_dongle		*right_dongle;
	long			last_compile_time;
	int				compile_counter;
	pthread_mutex_t	coder_mutex;
	t_simulation	*simulation;
}	t_coder;

typedef struct s_simulation
{
	int				total_coders;
	long			start_time;
	long			time_to_burnout;
	long			time_to_compile;
	long			time_to_debug;
	long			time_to_refactor;
	int				required_compiles;
	long			dongle_cooldown;
	char			*scheduler;
	int				is_over;
	pthread_mutex_t	is_over_mutex;
	pthread_mutex_t	write_lock;
	t_dongle		*dongles;
	t_coder			*coders;
}	t_simulation;

/* cycle.c */
void	*simulation_cycle(void *arg);

/* helpers.c */
long	get_current_time(void);
long	get_elapsed_time(t_simulation *sim);
int		get_is_over(t_simulation *sim);
void	set_is_over(t_simulation *sim, int value);
void	safe_sleep(t_coder *coder, long ms);

/* monitor.c */
void	*monitor_routine(void *arg);

/* parsing.c */
int		parse_data(char **data);

/* setup.c */
int		setup_simulation(t_simulation *sim, char **av);
void	create_threads(t_simulation *sim, pthread_t *monitor_thread);

/* clean_safe.c */
void	print_status(t_simulation *sim, int coder_id, char *text);
void	cleanup_simulation(t_simulation *sim);

/* scheduler.c */
void	add_to_queue(t_coder *coder, t_dongle *dongle);
void	remove_from_queue(t_coder *coder, t_dongle *dongle);
void	wait_dongle(t_coder *coder, t_dongle *dongle);
void	release_dongle(t_dongle *dongle);

/* single_coder.c */
void	*single_coder_cycle(t_coder *coder);

#endif
