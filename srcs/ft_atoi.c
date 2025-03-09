/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akunimot <akitig24@gmail.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 15:59:01 by akunimot          #+#    #+#             */
/*   Updated: 2025/03/09 16:00:01 by akunimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <../includes/philo.h>

static bool	is_cut(unsigned long current, unsigned long next,
		unsigned long base)
{
	unsigned long	cutoff;
	unsigned long	cutlim;

	cutoff = LONG_MAX / base;
	cutlim = LONG_MAX % base;
	if (current > cutoff || (current == cutoff && next > cutlim))
		return (true);
	else
		return (false);
}

static bool	is_check_flow(const char **str, int base, unsigned long *ul_v,
		bool *isneg)
{
	*isneg = false;
	*ul_v = 0;
	while (ft_isspace(**str))
		(*str)++;
	if (**str == '-')
		*isneg = true;
	if (**str == '-' || **str == '+')
		(*str)++;
	while (ft_isdigit(**str))
	{
		if (is_cut(*ul_v, (unsigned long)**str - '0', base))
			return (true);
		*ul_v = (*ul_v) * (unsigned long)base + (unsigned long)(**str - '0');
		(*str)++;
	}
	return (false);
}

long long	ft_strtol(const char *nptr, char **endptr, int base)
{
	unsigned long	ul_v;
	bool			error;
	bool			isneg;

	error = is_check_flow(&nptr, base, &ul_v, &isneg);
	if (endptr)
		*endptr = (char *)nptr;
	if (error)
	{
		errno = ERANGE;
		if (isneg == true)
			ul_v = (unsigned long)LONG_MIN;
		else
			ul_v = (unsigned long)LONG_MAX;
	}
	else if (isneg)
		ul_v = -ul_v;
	return ((long)ul_v);
}

int	ft_atoi(const char *str)
{
	return ((int)ft_strtol(str, NULL, 10));
}
