/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads_helpers.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akunimot <akitig24@gmail.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 15:30:00 by akunimot          #+#    #+#             */
/*   Updated: 2025/03/10 11:32:15 by akunimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

static int	take_forks_odd(t_philo *philo, int left, int right)
{
	t_data	*data;
	long	timestamp;

	data = philo->data;
	if (try_lock_mutex(&data->forks[left], data) < 0)
		return (-1);
	timestamp = get_now_ms(data->start);
	safe_print(data, "%ld %d has taken a fork\n", timestamp, philo->id);
	if (try_lock_mutex(&data->forks[right], data) < 0)
	{
		pthread_mutex_unlock(&data->forks[left]);
		return (-1);
	}
	timestamp = get_now_ms(data->start);
	safe_print(data, "%ld %d has taken a fork\n", timestamp, philo->id);
	return (0);
}

static int	take_forks_even(t_philo *philo, int left, int right)
{
	t_data	*data;
	long	timestamp;

	data = philo->data;
	if (try_lock_mutex(&data->forks[right], data) < 0)
		return (-1);
	timestamp = get_now_ms(data->start);
	safe_print(data, "%ld %d has taken a fork\n", timestamp, philo->id);
	if (try_lock_mutex(&data->forks[left], data) < 0)
	{
		pthread_mutex_unlock(&data->forks[right]);
		return (-1);
	}
	timestamp = get_now_ms(data->start);
	safe_print(data, "%ld %d has taken a fork\n", timestamp, philo->id);
	return (0);
}

int	take_forks(t_philo *philo, int left, int right)
{
	if (philo->id % 2 == 1)
		return (take_forks_odd(philo, left, right));
	else
		return (take_forks_even(philo, left, right));
}

void	release_forks(t_data *data, int left, int right)
{
	pthread_mutex_unlock(&data->forks[left]);
	pthread_mutex_unlock(&data->forks[right]);
}

int	check_death(t_data *data)
{
	int		i;
	long	current_time;

	i = 0;
	while (i < data->num_philos)
	{
		current_time = get_now_ms(data->start);
		if (!data->philos[i].is_eating && (current_time
				- data->philos[i].last_meal > data->time_to_die))
		{
			safe_print(data, "%ld %d died\n", current_time, data->philos[i].id);
			return (1);
		}
		i++;
	}
	return (0);
}
