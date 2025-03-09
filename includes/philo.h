/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akunimot <akitig24@gmail.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 15:01:23 by akunimot          #+#    #+#             */
/*   Updated: 2025/03/09 21:16:54 by akunimot         ###   ########.fr       */
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

#endif
