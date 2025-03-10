/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads_wrappers.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akunimot <akitig24@gmail.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 15:45:00 by akunimot          #+#    #+#             */
/*   Updated: 2025/03/10 12:16:13 by akunimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

int	take_forks_helper(t_philo *philo, int left, int right)
{
	return (take_forks(philo, left, right));
}

void	release_forks_helper(t_data *data, int left, int right)
{
	release_forks(data, left, right);
}

int	check_death_helper(t_data *data)
{
	return (check_death(data));
}

void	*single_philo_wrapper(void *arg)
{
	return (handle_single_philo((t_philo *)arg));
}
