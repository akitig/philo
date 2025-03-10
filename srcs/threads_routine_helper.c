/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads_routine_helper.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akunimot <akitig24@gmail.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 15:30:00 by akunimot          #+#    #+#             */
/*   Updated: 2025/03/10 11:26:02 by akunimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

/*
	Handle single philosopher case.
	For a single philosopher, take the fork, wait until death,
	and set simulation_stop.
*/
void	*handle_single_philo(t_philo *philo)
{
	t_data	*data;
	long	timestamp;

	data = philo->data;
	timestamp = get_now_ms(data->start);
	safe_print(data, "%ld %d has taken a fork\n", timestamp, philo->id);
	safe_sleep(data->time_to_die, data);
	timestamp = get_now_ms(data->start);
	safe_print(data, "%ld %d died\n", timestamp, philo->id);
	pthread_mutex_lock(&data->stop_mutex);
	data->simulation_stop = 1;
	pthread_mutex_unlock(&data->stop_mutex);
	return (NULL);
}

/*
	Perform the eating phase.
	Returns 0 on success, -1 if simulation should stop.
*/
static int	do_eating_phase(t_philo *philo, int left, int right)
{
	if (take_forks_helper(philo, left, right) < 0)
		return (-1);
	pthread_mutex_lock(&philo->data->meal_mutex);
	philo->last_meal = get_now_ms(philo->data->start);
	philo->is_eating = 1;
	pthread_mutex_unlock(&philo->data->meal_mutex);
	safe_print(philo->data, "%ld %d is eating\n",
		get_now_ms(philo->data->start), philo->id);
	safe_sleep(philo->data->time_to_eat, philo->data);
	if (!simulation_running(philo->data))
	{
		release_forks_helper(philo->data, left, right);
		return (-1);
	}
	pthread_mutex_lock(&philo->data->meal_mutex);
	philo->is_eating = 0;
	pthread_mutex_unlock(&philo->data->meal_mutex);
	philo->eat_count++;
	if (philo->data->num_times_eat > 0
		&& philo->eat_count >= philo->data->num_times_eat)
		check_finish(philo);
	release_forks_helper(philo->data, left, right);
	return (0);
}

/*
	Perform one full cycle of philosopher actions.
	Returns 0 on success, -1 if the cycle must abort.
*/
int	do_philosopher_cycle(t_philo *philo, int left, int right)
{
	t_data	*data;
	long	timestamp;
	int		ret;

	data = philo->data;
	timestamp = get_now_ms(data->start);
	safe_print(data, "%ld %d is thinking\n", timestamp, philo->id);
	ret = do_eating_phase(philo, left, right);
	if (ret < 0)
		return (-1);
	safe_print(data, "%ld %d is sleeping\n", get_now_ms(data->start),
		philo->id);
	safe_sleep(data->time_to_sleep, data);
	return (0);
}

/*
	Check monitor conditions.
	If all philosophers have finished or a death is detected,
	set simulation_stop and return (1); otherwise return 0.
*/
int	check_monitor_conditions(t_data *data)
{
	pthread_mutex_lock(&data->finish_mutex);
	pthread_mutex_lock(&data->stop_mutex);
	if (data->finished_count == data->num_philos || data->simulation_stop)
	{
		pthread_mutex_unlock(&data->stop_mutex);
		pthread_mutex_unlock(&data->finish_mutex);
		return (1);
	}
	pthread_mutex_unlock(&data->stop_mutex);
	pthread_mutex_unlock(&data->finish_mutex);
	pthread_mutex_lock(&data->meal_mutex);
	if (check_death_helper(data))
	{
		pthread_mutex_unlock(&data->meal_mutex);
		pthread_mutex_lock(&data->stop_mutex);
		data->simulation_stop = 1;
		pthread_mutex_unlock(&data->stop_mutex);
		return (1);
	}
	pthread_mutex_unlock(&data->meal_mutex);
	return (0);
}
