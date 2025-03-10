/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads_create_helper.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akunimot <akitig24@gmail.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 11:00:00 by akunimot          #+#    #+#             */
/*   Updated: 2025/03/10 12:20:02 by akunimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

void	create_single_thread(t_data *data)
{
	t_philo		*philo;
	pthread_t	thread;

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
	if (pthread_create(&thread, NULL, single_philo_wrapper, (void *)philo) != 0)
		safe_print_msg(data, "pthread_create failed");
	pthread_join(thread, NULL);
	free(philo);
}

static void	init_philos_array(t_data *data)
{
	int	i;

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
}

static void	create_philo_threads_loop(t_data *data, pthread_t *threads)
{
	int	i;

	i = 0;
	while (i < data->num_philos)
	{
		if (pthread_create(&threads[i], NULL, philosopher_routine,
				(void *)&data->philos[i]) != 0)
			safe_print_msg(data, "pthread_create failed");
		i++;
	}
}

static void	join_philo_threads(t_data *data, pthread_t *threads)
{
	int	i;

	i = 0;
	while (i < data->num_philos)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
}

void	create_multiple_philo(t_data *data)
{
	pthread_t	*threads;
	pthread_t	monitor;

	data->philos = malloc(sizeof(t_philo) * data->num_philos);
	if (!data->philos)
		return ;
	threads = malloc(sizeof(pthread_t) * data->num_philos);
	if (!threads)
	{
		free(data->philos);
		return ;
	}
	init_philos_array(data);
	create_philo_threads_loop(data, threads);
	if (pthread_create(&monitor, NULL, monitor_routine, data) != 0)
		safe_print_msg(data, "pthread_create for monitor failed");
	pthread_join(monitor, NULL);
	join_philo_threads(data, threads);
	free(threads);
	free(data->philos);
}
