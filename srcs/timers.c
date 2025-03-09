/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   timers.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akunimot <akitig24@gmail.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 17:37:48 by akunimot          #+#    #+#             */
/*   Updated: 2025/03/09 17:43:26 by akunimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <../includes/philo.h>

size_t	ft_strlen(const char *str)
{
	size_t	count;

	count = 0;
	while (*str)
	{
		count++;
		str++;
	}
	return (count);
}

int	ft_longlen(long n)
{
	int	i;

	i = 0;
	if (n < 0)
	{
		n = -n;
		i++;
	}
	while (n > 9)
	{
		n /= 10;
		i++;
	}
	i++;
	return (i);
}

void	ft_put(long n, char *str, int *i)
{
	if (n > 9)
	{
		ft_put(n / 10, str, i);
		ft_put(n % 10, str, i);
	}
	else
		str[(*i)++] = n + '0';
}

char	*ft_itoa(int n)
{
	long	num;
	char	*ans;
	int		i;

	num = n;
	ans = malloc(sizeof(char) * (ft_longlen(num) + 1));
	if (!ans)
		return (NULL);
	i = 0;
	if (num < 0)
	{
		ans[i++] = '-';
		num = -num;
	}
	ft_put(num, ans, &i);
	ans[i] = '\0';
	return (ans);
}

long	ft_atol(const char *str)
{
	long	result;
	long	sign;
	int		i;

	result = 0;
	sign = 1;
	i = 0;
	while ((str[i] >= 9 && str[i] <= 13) || str[i] == ' ')
		i++;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	while (str[i] && (str[i] >= '0' && str[i] <= '9'))
	{
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return (result * sign);
}

long	get_now_ms(struct timeval start)
{
	struct timeval	now;

	long sec, usec;
	gettimeofday(&now, NULL);
	sec = now.tv_sec - start.tv_sec;
	usec = now.tv_usec - start.tv_usec;
	return (sec * 1000 + usec / 1000);
}

void	get_right_now_ms(long next_time)
{
	struct timeval start;
	gettimeofday(&start, NULL);

	while (get_now_ms(start) < next_time)
		usleep(1000);
	char *time_str = ft_itoa((int)next_time);
	if (time_str)
	{
		write(1, time_str, ft_strlen(time_str));
		write(1, "\n", 1);
		free(time_str);
	}
}