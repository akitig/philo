/* ************************************************************************** */
/*                                                                            */
/*                            Norm compliant version                          */
/* ************************************************************************** */

#include "../includes/philo.h"
#include <pthread.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

/*
** simulation_running:
**   Lock stop_mutex, check simulation_stop flag.
*/
static int	simulation_running(t_data *data)
{
	pthread_mutex_lock(&data->stop_mutex);
	if (!data->simulation_stop)
	{
		pthread_mutex_unlock(&data->stop_mutex);
		return (1);
	}
	pthread_mutex_unlock(&data->stop_mutex);
	return (0);
}

/*
** safe_print:
**   Lock print_mutex and print if simulation_stop is not set.
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
** safe_print_msg:
**   Print a simple message if simulation_stop is not set.
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
** safe_sleep:
**   Sleep for msec while checking simulation_stop every 100µs.
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
** try_lock_mutex:
**   Try locking mutex while checking simulation_running.
*/
static int	try_lock_mutex(pthread_mutex_t *mutex, t_data *data)
{
	while (pthread_mutex_trylock(mutex) != 0)
	{
		if (!simulation_running(data))
			return (-1);
		usleep(50);
	}
	return (0);
}

/*
** lock_and_print:
**   Lock fork at given index and print message.
*/
static int	lock_and_print(t_philo *philo, int fork_index)
{
	long	time;

	if (try_lock_mutex(&philo->data->forks[fork_index], philo->data) < 0)
		return (-1);
	time = get_now_ms(philo->data->start);
	safe_print(philo->data, "%ld %d has taken a fork\n", time, philo->id);
	return (0);
}

/*
** lock_forks:
**   Lock two forks based on philosopher id parity.
*/
static int	lock_forks(t_philo *philo, int left, int right)
{
	if (philo->id % 2 == 1)
	{
		if (lock_and_print(philo, left) < 0)
			return (-1);
		if (lock_and_print(philo, right) < 0)
		{
			pthread_mutex_unlock(&philo->data->forks[left]);
			return (-1);
		}
	}
	else
	{
		if (lock_and_print(philo, right) < 0)
			return (-1);
		if (lock_and_print(philo, left) < 0)
		{
			pthread_mutex_unlock(&philo->data->forks[right]);
			return (-1);
		}
	}
	return (0);
}

/*
** update_meal:
**   Update last_meal and set is_eating flag.
*/
static void	update_meal(t_philo *philo)
{
	pthread_mutex_lock(&philo->data->meal_mutex);
	philo->last_meal = get_now_ms(philo->data->start);
	philo->is_eating = 1;
	pthread_mutex_unlock(&philo->data->meal_mutex);
}

/*
** check_finished_count:
**   Update finished_count and set simulation_stop if necessary.
*/
static void	check_finished_count(t_data *data)
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

/*
** finish_eating:
**   Reset is_eating, update eat_count, and unlock forks.
*/
static int	finish_eating(t_philo *philo, int left, int right)
{
	if (!simulation_running(philo->data))
	{
		pthread_mutex_unlock(&philo->data->forks[left]);
		pthread_mutex_unlock(&philo->data->forks[right]);
		return (-1);
	}
	pthread_mutex_lock(&philo->data->meal_mutex);
	philo->is_eating = 0;
	pthread_mutex_unlock(&philo->data->meal_mutex);
	philo->eat_count++;
	if (philo->data->num_times_eat > 0
		&& philo->eat_count >= philo->data->num_times_eat)
	{
		check_finished_count(philo->data);
	}
	pthread_mutex_unlock(&philo->data->forks[left]);
	pthread_mutex_unlock(&philo->data->forks[right]);
	return (0);
}

/*
** handle_single_philo:
**   Special handling for one philosopher.
*/
static int	handle_single_philo(t_philo *philo)
{
	long	timestamp;

	timestamp = get_now_ms(philo->data->start);
	safe_print(philo->data, "%ld %d has taken a fork\n", timestamp, philo->id);
	safe_sleep(philo->data->time_to_die, philo->data);
	timestamp = get_now_ms(philo->data->start);
	safe_print(philo->data, "%ld %d died\n", timestamp, philo->id);
	pthread_mutex_lock(&philo->data->stop_mutex);
	philo->data->simulation_stop = 1;
	pthread_mutex_unlock(&philo->data->stop_mutex);
	return (0);
}

/*
** philosopher_iteration:
**   One iteration of philosopher actions.
*/
static int	philosopher_iteration(t_philo *philo, int left, int right)
{
	long	timestamp;
	t_data	*data;

	data = philo->data;
	timestamp = get_now_ms(data->start);
	safe_print(data, "%ld %d is thinking\n", timestamp, philo->id);
	if (lock_forks(philo, left, right) < 0)
		return (-1);
	update_meal(philo);
	timestamp = get_now_ms(data->start);
	safe_print(data, "%ld %d is eating\n", timestamp, philo->id);
	safe_sleep(data->time_to_eat, data);
	if (finish_eating(philo, left, right) < 0)
		return (-1);
	timestamp = get_now_ms(data->start);
	safe_print(data, "%ld %d is sleeping\n", timestamp, philo->id);
	safe_sleep(data->time_to_sleep, data);
	return (0);
}

/*
** philosopher_routine:
**   Routine for each philosopher thread.
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
	{
		handle_single_philo(philo);
		return (NULL);
	}
	left = philo->id - 1;
	right = philo->id % data->num_philos;
	while (simulation_running(data))
	{
		if (philosopher_iteration(philo, left, right) < 0)
			break ;
	}
	return (NULL);
}

/*
** check_death:
**   Check if any philosopher has died.
*/
static int	check_death(t_data *data, long current_time)
{
	int	i;

	i = 0;
	while (i < data->num_philos)
	{
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

/*
** monitor_should_break:
**   Check finish and stop conditions.
*/
static int	monitor_should_break(t_data *data)
{
	int	finished;

	pthread_mutex_lock(&data->finish_mutex);
	finished = (data->finished_count == data->num_philos);
	pthread_mutex_unlock(&data->finish_mutex);
	if (finished)
		return (1);
	pthread_mutex_lock(&data->stop_mutex);
	if (data->simulation_stop)
	{
		pthread_mutex_unlock(&data->stop_mutex);
		return (1);
	}
	pthread_mutex_unlock(&data->stop_mutex);
	return (0);
}

/*
** monitor_routine:
**   Monitor thread to check philosopher status.
*/
void	*monitor_routine(void *arg)
{
	t_data	*data;
	long	current_time;
	int		death;

	data = (t_data *)arg;
	while (simulation_running(data))
	{
		if (monitor_should_break(data))
			break ;
		pthread_mutex_lock(&data->meal_mutex);
		current_time = get_now_ms(data->start);
		death = check_death(data, current_time);
		pthread_mutex_unlock(&data->meal_mutex);
		if (death)
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

/*
** create_single_philo:
**   Create thread for a single philosopher.
*/
static void	create_single_philo(t_data *data)
{
	pthread_t	thread;

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
	if (pthread_create(&thread, NULL, philosopher_routine,
			(void *)&data->philos[0]) != 0)
		safe_print_msg(data, "pthread_create failed");
	pthread_join(thread, NULL);
	free(data->philos);
}

/*
** create_philo_threads_loop:
**   Loop to create philosopher threads.
*/
static void	create_philo_threads_loop(t_data *data, pthread_t *threads)
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
		if (pthread_create(&threads[i], NULL, philosopher_routine,
				(void *)&data->philos[i]) != 0)
			safe_print_msg(data, "pthread_create failed");
		i++;
	}
}

/*
** create_multiple_philos:
**   Create threads for multiple philosophers.
*/
static void	create_philo_threads(t_data *data, pthread_t *threads)
{
	create_philo_threads_loop(data, threads);
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

static void	create_multiple_philos(t_data *data)
{
	pthread_t	*threads;
	pthread_t	monitor;

	/* Allocate philosopher array for multiple philosophers */
	data->philos = malloc(sizeof(t_philo) * data->num_philos);
	if (!data->philos)
		return ;
	threads = malloc(sizeof(pthread_t) * data->num_philos);
	if (!threads)
	{
		free(data->philos);
		return ;
	}
	create_philo_threads(data, threads);
	if (pthread_create(&monitor, NULL, monitor_routine, data) != 0)
		safe_print_msg(data, "pthread_create for monitor failed");
	pthread_join(monitor, NULL);
	join_philo_threads(data, threads);
	free(threads);
	free(data->philos);
}

/*
** create_philos_threads:
**   Create philosopher and monitor threads.
*/
void	create_philos_threads(t_data *data)
{
	if (data->num_philos == 1)
		create_single_philo(data);
	else
		create_multiple_philos(data);
}

static int	validate_args(char **av)
{
	int	arg1;
	int	arg2;
	int	arg3;
	int	arg4;

	arg1 = ft_atoi(av[1]);
	arg2 = ft_atoi(av[2]);
	arg3 = ft_atoi(av[3]);
	arg4 = ft_atoi(av[4]);
	if (arg1 <= 0 || arg2 <= 0 || arg3 <= 0 || arg4 <= 0)
	{
		printf("Error: invalid argument\n");
		return (0);
	}
	if (av[5] && ft_atoi(av[5]) <= 0)
	{
		printf("Error: invalid argument\n");
		return (0);
	}
	return (1);
}

/*
** init_data:
**   Initialize simulation data.
*/
bool	init_data(t_data *data, char **av)
{
	struct timeval	tv;

	if (!validate_args(av))
		return (false);
	data->num_philos = ft_atoi(av[1]);
	data->time_to_die = ft_atoi(av[2]);
	data->time_to_eat = ft_atoi(av[3]);
	data->time_to_sleep = ft_atoi(av[4]);
	if (av[5])
		data->num_times_eat = ft_atoi(av[5]);
	else
		data->num_times_eat = -1;
	gettimeofday(&tv, NULL);
	data->start = tv;
	data->simulation_stop = 0;
	data->finished_count = 0;
	pthread_mutex_init(&data->print_mutex, NULL);
	return (true);
}

/*
** init_fork_mutexes:
**   Initialize fork mutexes.
*/
static bool	init_fork_mutexes(t_data *data)
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
** init_other_mutexes:
**   Initialize other mutexes (excluding print_mutex).
*/
static bool	init_other_mutexes(t_data *data)
{
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
	return (true);
}

/*
** init_forks:
**   Initialize fork and other mutexes.
*/
bool	init_forks(t_data *data)
{
	data->forks = malloc(sizeof(pthread_mutex_t) * data->num_philos);
	if (!data->forks)
	{
		printf("Failed to allocate forks\n");
		return (false);
	}
	if (!init_fork_mutexes(data))
		return (false);
	if (!init_other_mutexes(data))
		return (false);
	return (true);
}

/*
** philo:
**   Main simulation execution.
*/
int	philo(char **av)
{
	t_data	data;
	int		i;

	if (!init_data(&data, av))
		return (1);
	if (!init_forks(&data))
		return (1);
	create_philos_threads(&data);
	i = 0;
	while (i < data.num_philos)
	{
		pthread_mutex_destroy(&data.forks[i]);
		i++;
	}
	pthread_mutex_destroy(&data.meal_mutex);
	pthread_mutex_destroy(&data.finish_mutex);
	pthread_mutex_destroy(&data.stop_mutex);
	pthread_mutex_destroy(&data.print_mutex);
	free(data.forks);
	return (0);
}

/*
** main:
**   Program entry point.
*/
int	main(int ac, char **av)
{
	if (ac == 5 || ac == 6)
		return (philo(av));
	else
	{
		printf("Usage: ./philo number_of_philosophers time_to_die ");
		printf("time_to_eat time_to_sleep ");
		printf("[number_of_times_each_philosopher_must_eat]\n");
		return (1);
	}
}
