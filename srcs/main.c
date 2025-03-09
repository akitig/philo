/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akunimot <akitig24@gmail.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 14:59:40 by akunimot          #+#    #+#             */
/*   Updated: 2025/03/09 17:21:05 by akunimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <../includes/philo.h>

void	*philosopher_routine(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	printf("philo %d start\n", philo->id);
	while (1)
	{
		printf("philosopher %d is thinking\n", philo->id);
		usleep(500000);
	}
	return (NULL);
}

void	create_philos_threads(t_data data)
{
	pthread_t	*threads;
	t_philo		*philos;
	int			i;

	printf("num:%d\n", data.num_philos);
	threads = malloc(sizeof(pthread_t) * data.num_philos);
	if (!threads)
		return ;
	philos = malloc(sizeof(t_philo) * data.num_philos);
	if (!philos)
	{
		free(threads);
		return ;
	}
	i = 0;
	while (i < data.num_philos)
	{
		philos[i].id = i + 1;
		philos[i].data = &data;
		if (pthread_create(&threads[i], NULL, philosopher_routine,
				(void *)&philos[i]) != 0)
		{
			printf("pthread_create failed");
		}
		i++;
	}
	i = 0;
	while (i < data.num_philos)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
	free(threads);
	free(philos);
}

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
	create_philos_threads(data);
	pthread_mutex_destroy(&mutex.lock);
	return (0);
}

// ./philo number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]
int	main(int ac, char **av)
{
	if (ac == 5)
	{
		printf("philo must loop infinity\n");
		return (philo(av));
	}

	else
		printf("num of args = KO\n");
	return (0);
}