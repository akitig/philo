/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akunimot <akitig24@gmail.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 15:01:23 by akunimot          #+#    #+#             */
/*   Updated: 2025/03/09 17:21:32 by akunimot         ###   ########.fr       */
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
# include <unistd.h>

typedef struct s_mutex
{
	int				counter;
	pthread_mutex_t	lock;

}					t_mutex;

typedef struct s_data
{
	int				num_philos;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				num_times_eat;
}					t_data;

typedef struct s_philo
{
	int				id;
	t_data			*data;

}					t_philo;

// ft_is.c
int					ft_isdigit(int c);
int					ft_isspace(char c);

// ft_atoi.c
int					ft_atoi(const char *str);

#endif