/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akunimot <akitig24@gmail.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 14:59:40 by akunimot          #+#    #+#             */
/*   Updated: 2025/03/10 07:47:22 by akunimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"
#include <pthread.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

/*
 * simulation_running:
 *    stop_mutex をロックして simulation_stop フラグを確認し、
 *    シミュレーションが継続中なら 1 を返す。
 */
static int	simulation_running(t_data *data)
{
	int	running;

	pthread_mutex_lock(&data->stop_mutex);
	running = !data->simulation_stop;
	pthread_mutex_unlock(&data->stop_mutex);
	return (running);
}

/*
 * safe_print:
 *    print_mutex をロックし、simulation_stop がセットされていなければ
 *    指定のフォーマットで出力する。
 */
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

/*
 * safe_print_msg:
 *    単純なメッセージを出力する。
 */
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

/*
 * safe_sleep:
 *    指定されたミリ秒だけ、100µs ごとに simulation_stop をチェックしながら待機する。
 *    simulation_stop がセットされた場合、すぐに待機を中断する。
 */
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

/*
 * try_lock:
 *    指定のミューテックスを、simulation_stop をチェックしながら trylock する。
 *    取得できたら 0 を返す。simulation_stop がセットされたら -1 を返す。
 */
static int	try_lock_mutex(pthread_mutex_t *mutex, t_data *data)
{
	while (pthread_mutex_trylock(mutex) != 0)
	{
		if (!simulation_running(data))
			return (-1);
		usleep(50); // 50µs ごとに再試行
	}
	return (0);
}

/* 哲学者スレッドのルーチン */
void	*philosopher_routine(void *arg)
{
	t_philo	*philo;
	t_data	*data;
	long	timestamp;

	philo = (t_philo *)arg;
	data = philo->data;
	int left, right;
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	/* 偶数番号の哲学者は初期に短い遅延（例：200µs） */
	if (philo->id % 2 == 0)
		usleep(200);
	if (data->num_philos == 1)
	{
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
	left = philo->id - 1;
	right = philo->id % data->num_philos;
	while (simulation_running(data))
	{
		timestamp = get_now_ms(data->start);
		safe_print(data, "%ld %d is thinking\n", timestamp, philo->id);
		if (philo->id % 2 == 1)
		{
			if (try_lock_mutex(&data->forks[left], data) < 0)
				break ;
			safe_print(data, "%ld %d has taken a fork\n",
				get_now_ms(data->start), philo->id);
			if (try_lock_mutex(&data->forks[right], data) < 0)
			{
				pthread_mutex_unlock(&data->forks[left]);
				break ;
			}
			safe_print(data, "%ld %d has taken a fork\n",
				get_now_ms(data->start), philo->id);
		}
		else
		{
			if (try_lock_mutex(&data->forks[right], data) < 0)
				break ;
			safe_print(data, "%ld %d has taken a fork\n",
				get_now_ms(data->start), philo->id);
			if (try_lock_mutex(&data->forks[left], data) < 0)
			{
				pthread_mutex_unlock(&data->forks[right]);
				break ;
			}
			safe_print(data, "%ld %d has taken a fork\n",
				get_now_ms(data->start), philo->id);
		}
		/* 食事開始直前に last_meal を更新し、is_eating を 1 に設定 */
		pthread_mutex_lock(&data->meal_mutex);
		philo->last_meal = get_now_ms(data->start);
		philo->is_eating = 1;
		pthread_mutex_unlock(&data->meal_mutex);
		timestamp = get_now_ms(data->start);
		safe_print(data, "%ld %d is eating\n", timestamp, philo->id);
		safe_sleep(data->time_to_eat, data);
		if (!simulation_running(data))
		{
			pthread_mutex_unlock(&data->forks[left]);
			pthread_mutex_unlock(&data->forks[right]);
			break ;
		}
		pthread_mutex_lock(&data->meal_mutex);
		philo->is_eating = 0;
		pthread_mutex_unlock(&data->meal_mutex);
		philo->eat_count++;
		if (data->num_times_eat > 0 && philo->eat_count >= data->num_times_eat)
		{
			pthread_mutex_lock(&data->finish_mutex);
			data->finished_count++;
			if (data->finished_count == data->num_philos)
			{
				pthread_mutex_lock(&data->stop_mutex);
				data->simulation_stop = 1;
				pthread_mutex_unlock(&data->stop_mutex);
			}
			pthread_mutex_unlock(&data->finish_mutex);
		}
		pthread_mutex_unlock(&data->forks[left]);
		pthread_mutex_unlock(&data->forks[right]);
		timestamp = get_now_ms(data->start);
		safe_print(data, "%ld %d is sleeping\n", timestamp, philo->id);
		safe_sleep(data->time_to_sleep, data);
		if (!simulation_running(data))
			break ;
	}
	return (NULL);
}

/* モニタースレッド：各哲学者の死亡を監視 */
void	*monitor_routine(void *arg)
{
	t_data	*data;
	int		i;
	long	current_time;
	int		death_detected;

	data = (t_data *)arg;
	death_detected = 0;
	while (simulation_running(data))
	{
		pthread_mutex_lock(&data->finish_mutex);
		if (data->finished_count == data->num_philos)
		{
			pthread_mutex_unlock(&data->finish_mutex);
			break ;
		}
		pthread_mutex_unlock(&data->finish_mutex);
		pthread_mutex_lock(&data->stop_mutex);
		if (data->simulation_stop)
		{
			pthread_mutex_unlock(&data->stop_mutex);
			break ;
		}
		pthread_mutex_unlock(&data->stop_mutex);
		death_detected = 0;
		pthread_mutex_lock(&data->meal_mutex);
		for (i = 0; i < data->num_philos; i++)
		{
			current_time = get_now_ms(data->start);
			if (!data->philos[i].is_eating && (current_time
					- data->philos[i].last_meal > data->time_to_die))
			{
				death_detected = 1;
				safe_print(data, "%ld %d died\n", current_time,
					data->philos[i].id);
				break ;
			}
		}
		pthread_mutex_unlock(&data->meal_mutex);
		if (death_detected)
		{
			pthread_mutex_lock(&data->stop_mutex);
			data->simulation_stop = 1;
			pthread_mutex_unlock(&data->stop_mutex);
			break ;
		}
		usleep(500);
	}
	return (NULL);
}

/* 各哲学者のスレッドおよびモニタースレッドの生成 */
void	create_philos_threads(t_data *data)
{
	pthread_t	*threads;
	pthread_t	monitor;
	int			i;
	pthread_t	thread;

	if (data->num_philos == 1)
	{
		data->philos = malloc(sizeof(t_philo));
		if (!data->philos)
			return ;
		data->philos[0].id = 1;
		data->philos[0].eat_count = 0;
		data->philos[0].is_eating = 0;
		data->philos[0].data = data;
		pthread_mutex_lock(&data->meal_mutex);
		data->philos[0].last_meal = 0;
		pthread_mutex_unlock(&data->meal_mutex);
		{
			if (pthread_create(&thread, NULL, philosopher_routine,
					(void *)&data->philos[0]) != 0)
				safe_print_msg(data, "pthread_create failed");
			pthread_join(thread, NULL);
		}
		free(data->philos);
		return ;
	}
	data->philos = malloc(sizeof(t_philo) * data->num_philos);
	if (!data->philos)
		return ;
	threads = malloc(sizeof(pthread_t) * data->num_philos);
	if (!threads)
	{
		free(data->philos);
		return ;
	}
	for (i = 0; i < data->num_philos; i++)
	{
		data->philos[i].id = i + 1;
		data->philos[i].eat_count = 0;
		data->philos[i].is_eating = 0;
		data->philos[i].data = data;
		pthread_mutex_lock(&data->meal_mutex);
		data->philos[i].last_meal = 0;
		pthread_mutex_unlock(&data->meal_mutex);
		if (pthread_create(&threads[i], NULL, philosopher_routine,
				(void *)&data->philos[i]) != 0)
			safe_print_msg(data, "pthread_create failed");
	}
	if (pthread_create(&monitor, NULL, monitor_routine, data) != 0)
		safe_print_msg(data, "pthread_create for monitor failed");
	pthread_join(monitor, NULL);
	for (i = 0; i < data->num_philos; i++)
	{
		pthread_join(threads[i], NULL);
	}
	free(threads);
	free(data->philos);
}

/* 初期データの設定 */
bool	init_data(t_data *data, char **av)
{
	struct timeval	tv;

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
	gettimeofday(&tv, NULL);
	data->start = tv;
	data->simulation_stop = 0;
	data->finished_count = 0;
	pthread_mutex_init(&data->print_mutex, NULL);
	return (true);
}

/* forks と各ミューテックスの初期化 */
bool	init_forks(t_data *data)
{
	int	i;

	data->forks = malloc(sizeof(pthread_mutex_t) * data->num_philos);
	if (!data->forks)
	{
		printf("Failed to allocate forks\n");
		return (false);
	}
	for (i = 0; i < data->num_philos; i++)
	{
		if (pthread_mutex_init(&data->forks[i], NULL) != 0)
		{
			printf("Failed to initialize fork mutex\n");
			return (false);
		}
	}
	if (pthread_mutex_init(&data->meal_mutex, NULL) != 0)
	{
		printf("Failed to initialize meal mutex\n");
		return (false);
	}
	if (pthread_mutex_init(&data->finish_mutex, NULL) != 0)
	{
		printf("Failed to initialize finish mutex\n");
		return (false);
	}
	if (pthread_mutex_init(&data->stop_mutex, NULL) != 0)
	{
		printf("Failed to initialize stop mutex\n");
		return (false);
	}
	if (pthread_mutex_init(&data->print_mutex, NULL) != 0)
	{
		printf("Failed to initialize print mutex\n");
		return (false);
	}
	return (true);
}

/* シミュレーション開始 */
int	philo(char **av)
{
	t_data	data;
	int		i;

	if (!init_data(&data, av))
		return (1);
	if (!init_forks(&data))
		return (1);
	create_philos_threads(&data);
	for (i = 0; i < data.num_philos; i++)
	{
		pthread_mutex_destroy(&data.forks[i]);
	}
	pthread_mutex_destroy(&data.meal_mutex);
	pthread_mutex_destroy(&data.finish_mutex);
	pthread_mutex_destroy(&data.stop_mutex);
	pthread_mutex_destroy(&data.print_mutex);
	free(data.forks);
	return (0);
}

int	main(int ac, char **av)
{
	if (ac == 5 || ac == 6)
		return (philo(av));
	else
	{
		printf("Usage: ./philo number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]\n");
		return (1);
	}
}
