/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akunimot <akitig24@gmail.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 15:40:00 by akunimot          #+#    #+#             */
/*   Updated: 2025/03/10 12:51:22 by akunimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

int	simulation_running(t_data *data)
{
	int	running;

	pthread_mutex_lock(&data->stop_mutex);
	running = !data->simulation_stop;
	pthread_mutex_unlock(&data->stop_mutex);
	return (running);
}

void	safe_print(t_data *data, const char *format, long time, int id)
{
	int	stop;

	pthread_mutex_lock(&data->print_mutex);
	pthread_mutex_lock(&data->stop_mutex);
	stop = data->simulation_stop;
	pthread_mutex_unlock(&data->stop_mutex);
	if (!stop)
		printf(format, time, id);
	pthread_mutex_unlock(&data->print_mutex);
}

void	safe_print_msg(t_data *data, const char *msg)
{
	int	stop;

	pthread_mutex_lock(&data->print_mutex);
	pthread_mutex_lock(&data->stop_mutex);
	stop = data->simulation_stop;
	pthread_mutex_unlock(&data->stop_mutex);
	if (!stop)
		printf("%s\n", msg);
	pthread_mutex_unlock(&data->print_mutex);
}

void	safe_sleep(long msec, t_data *data)
{
	long	start_time;

	start_time = get_now_ms(data->start);
	while ((get_now_ms(data->start) - start_time) < msec)
	{
		pthread_mutex_lock(&data->stop_mutex);
		if (data->simulation_stop)
		{
			pthread_mutex_unlock(&data->stop_mutex);
			return ;
		}
		pthread_mutex_unlock(&data->stop_mutex);
		usleep(100);
	}
}

// int	try_lock_mutex(pthread_mutex_t *mutex, t_data *data)
// {
// 	// while (pthread_mutex_trylock(mutex) != 0)
// 	// {
// 	// 	if (!simulation_running(data))
// 	// 		return (-1);
// 	// 	usleep(50);
// 	// }
// 	// return (0);

// }
int	try_lock_mutex(pthread_mutex_t *mutex, t_data *data)
{
	(void)data;
	pthread_mutex_lock(mutex);
	return (0);
}
