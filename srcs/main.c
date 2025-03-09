/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akunimot <akitig24@gmail.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 14:59:40 by akunimot          #+#    #+#             */
/*   Updated: 2025/03/09 15:55:10 by akunimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <../includes/philo.h>

// void	create_philos_threads(void)

bool	init_data(t_data *data, char **av)
{
	data->num_philos = ft_atoi(av[1]);
	data->time_to_die = ft_atoi(av[2]);
	data->time_to_eat = ft_atoi(av[3]);
	data->time_to_sleep = ft_atoi(av[4]);
	return (0);
}

bool	init_mutex(t_mutex *mutex)
{
	mutex->counter = 0;
	if (pthread_mutex_init(&mutex->lock, NULL) != 0)
	{
		printf("Failed to init mutex\n");
		return (1);
	}
	return (0);
}

int	philo(char **av)
{
	t_mutex	mutex;
	t_data	data;

	if (init_data(&data, av))
		return (1);
	if (init_mutex(&mutex))
		return (1);
	printf("num_philos :%d\n", data.num_philos);
	// create_philos_threads(data);
	pthread_mutex_destroy(&mutex.lock);
	return (0);
}

// ./philo number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]
int	main(int ac, char **av)
{
	(void)av;
	if (ac == 5)
	{
		printf("philo must loop infinity\n");
		return (philo(av));
	}

	else
		printf("num of args = KO\n");
	return (0);
}