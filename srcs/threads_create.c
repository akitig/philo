/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads_create.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akunimot <akitig24@gmail.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 11:00:00 by akunimot          #+#    #+#             */
/*   Updated: 2025/03/10 12:09:14 by akunimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

void	create_philos_threads(t_data *data)
{
	t_philo		*philo;
	pthread_t	thread;
	pthread_t	*threads;
	pthread_t	monitor;
	int			i;

	if (data->num_philos == 1)
	{
		philo = malloc(sizeof(t_philo));
		if (!philo)
			return ;
		philo->id = 1;
		philo->eat_count = 0;
		philo->is_eating = 0;
		philo->data = data;
		pthread_mutex_lock(&data->meal_mutex);
		philo->last_meal = 0;
		pthread_mutex_unlock(&data->meal_mutex);
		if (pthread_create(&thread, NULL,
				(void *(*)(void *))handle_single_philo, (void *)philo) != 0)
			safe_print_msg(data, "pthread_create failed");
		pthread_join(thread, NULL);
		free(philo);
	}
	else
	{
		data->philos = malloc(sizeof(t_philo) * data->num_philos);
		if (!data->philos)
			return ;
		threads = malloc(sizeof(pthread_t) * data->num_philos);
		if (!threads)
		{
			free(data->philos);
			return ;
		}
		i = 0;
		while (i < data->num_philos)
		{
			data->philos[i].id = i + 1;
			data->philos[i].eat_count = 0;
			data->philos[i].is_eating = 0;
			data->philos[i].data = data;
			pthread_mutex_lock(&data->meal_mutex);
			data->philos[i].last_meal = 0;
			pthread_mutex_unlock(&data->meal_mutex);
			i++;
		}
		i = 0;
		while (i < data->num_philos)
		{
			if (pthread_create(&threads[i], NULL, philosopher_routine,
					(void *)&data->philos[i]) != 0)
				safe_print_msg(data, "pthread_create failed");
			i++;
		}
		if (pthread_create(&monitor, NULL, monitor_routine, data) != 0)
			safe_print_msg(data, "pthread_create for monitor failed");
		pthread_join(monitor, NULL);
		i = 0;
		while (i < data->num_philos)
		{
			pthread_join(threads[i], NULL);
			i++;
		}
		free(threads);
		free(data->philos);
	}
}
