/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akunimot <akitig24@gmail.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 14:59:40 by akunimot          #+#    #+#             */
/*   Updated: 2025/03/09 15:02:34 by akunimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <../includes/philo.h>

// ./philo number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]
int	main(int ac, char **av)
{
	(void)av;
	if (ac == 5)
	{
		printf("philo must loop infinity\n");
	}

	else
		printf("num of args = KO\n");
	return (0);
}