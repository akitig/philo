/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads_routine.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akunimot <akitig24@gmail.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 15:20:00 by akunimot          #+#    #+#             */
/*   Updated: 2025/03/10 11:26:36 by akunimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

/*
	philosopher_routine:
	This routine now simply calls a helper (do_philosopher_cycle)
	defined in threads_helpers.c to keep this function under 25 lines.
*/
void	*philosopher_routine(void *arg)
{
	t_philo	*philo;
	t_data	*data;
	int		left;
	int		right;

	philo = (t_philo *)arg;
	data = philo->data;
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	if (philo->id % 2 == 0)
		usleep(200);
	if (data->num_philos == 1)
		return (handle_single_philo(philo));
	left = philo->id - 1;
	right = philo->id % data->num_philos;
	while (simulation_running(data))
	{
		if (do_philosopher_cycle(philo, left, right) < 0)
			break ;
	}
	return (NULL);
}

/*
	monitor_routine:
	This routine simply calls a helper to check monitor conditions.
*/
void	*monitor_routine(void *arg)
{
	t_data	*data;

	data = (t_data *)arg;
	while (simulation_running(data))
	{
		if (check_monitor_conditions(data))
			break ;
		usleep(500);
	}
	return (NULL);
}
