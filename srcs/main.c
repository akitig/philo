#include <stdio.h>
#include <stdlib.h>

// ./philo number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]
int	main(int ac, char **av)
{
	(void)av;
	if (ac == 6)
		printf("num of args = OK\n");
	else
		printf("num of args = KO\n");
	return (0);
}