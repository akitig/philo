/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akunimot <akitig24@gmail.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 15:10:00 by akunimot          #+#    #+#             */
/*   Updated: 2025/03/10 11:16:06 by akunimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

/*
	Validate arguments and assign basic values.
*/
static bool	validate_args(t_data *data, char **av)
{
	data->num_philos = ft_atoi(av[1]);
	data->time_to_die = ft_atoi(av[2]);
	data->time_to_eat = ft_atoi(av[3]);
	data->time_to_sleep = ft_atoi(av[4]);
	if (data->num_philos <= 0 || data->time_to_die <= 0
		|| data->time_to_eat <= 0 || data->time_to_sleep <= 0)
	{
		printf("Error: invalid argument\n");
		return (false);
	}
	if (av[5])
	{
		data->num_times_eat = ft_atoi(av[5]);
		if (data->num_times_eat <= 0)
		{
			printf("Error: invalid argument\n");
			return (false);
		}
	}
	else
		data->num_times_eat = -1;
	return (true);
}

/*
	Initialize simulation data.
*/
bool	init_data(t_data *data, char **av)
{
	struct timeval	tv;

	if (!validate_args(data, av))
		return (false);
	gettimeofday(&tv, NULL);
	data->start = tv;
	data->simulation_stop = 0;
	data->finished_count = 0;
	pthread_mutex_init(&data->print_mutex, NULL);
	return (true);
}

/*
	Helper: initialize one mutex.
*/
static bool	init_mutex(pthread_mutex_t *mutex, char *err)
{
	if (pthread_mutex_init(mutex, NULL) != 0)
	{
		printf("%s\n", err);
		return (false);
	}
	return (true);
}

/*
	Helper: initialize all fork mutexes.
*/
static bool	init_forks_array(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->num_philos)
	{
		if (pthread_mutex_init(&data->forks[i], NULL) != 0)
		{
			printf("Failed to initialize fork mutex\n");
			return (false);
		}
		i++;
	}
	return (true);
}

/*
	Initialize all required mutexes and allocate fork mutexes.
*/
bool	init_forks(t_data *data)
{
	data->forks = malloc(sizeof(pthread_mutex_t) * data->num_philos);
	if (!data->forks)
	{
		printf("Failed to allocate forks\n");
		return (false);
	}
	if (!init_forks_array(data))
		return (false);
	if (!init_mutex(&data->meal_mutex, "Failed to initialize meal mutex"))
		return (false);
	if (!init_mutex(&data->finish_mutex, "Failed to initialize finish mutex"))
		return (false);
	if (!init_mutex(&data->stop_mutex, "Failed to initialize stop mutex"))
		return (false);
	if (!init_mutex(&data->print_mutex, "Failed to initialize print mutex"))
		return (false);
	return (true);
}
