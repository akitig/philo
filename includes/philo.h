/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akunimot <akitig24@gmail.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 15:01:23 by akunimot          #+#    #+#             */
/*   Updated: 2025/03/10 11:42:28 by akunimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <errno.h>
# include <limits.h>
# include <pthread.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>

typedef struct s_data	t_data;

typedef struct s_philo
{
	int					id;
	int					eat_count;
	long				last_meal;
	t_data				*data;
	int					is_eating;
}						t_philo;

typedef struct s_data
{
	int					num_philos;
	int					time_to_die;
	int					time_to_eat;
	int					time_to_sleep;
	int					num_times_eat;
	struct timeval		start;
	pthread_mutex_t		*forks;
	pthread_mutex_t		meal_mutex;
	pthread_mutex_t		stop_mutex;
	pthread_mutex_t		finish_mutex;
	pthread_mutex_t		print_mutex;

	t_philo				*philos;
	int					simulation_stop;
	int					finished_count;
}						t_data;

/* ft_is.c */
int						ft_isdigit(int c);
int						ft_isspace(char c);

/* ft_atoi.c */
int						ft_atoi(const char *str);

/* timers.c */
long					get_now_ms(struct timeval start);


/* philo.c */
int						philo(char **av);

/* init.c */
bool					init_data(t_data *data, char **av);
bool					init_forks(t_data *data);

/* threads_helpers.c */
int						take_forks(t_philo *philo, int left, int right);
void					release_forks(t_data *data, int left, int right);
int						check_death(t_data *data);

/* threads_routine_helper.c */
void					*handle_single_philo(t_philo *philo);
int						do_philosopher_cycle(t_philo *philo, int left,
							int right);
int						check_monitor_conditions(t_data *data);

/* threads_routine.c */
void					*philosopher_routine(void *arg);
void					*monitor_routine(void *arg);

/*threads_wrappers.c*/
int						take_forks_helper(t_philo *philo, int left, int right);
void					release_forks_helper(t_data *data, int left, int right);
int						check_death_helper(t_data *data);
#endif
