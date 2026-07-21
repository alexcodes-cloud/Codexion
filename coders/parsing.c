/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-azim <yel-azim@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 18:29:41 by yel-azim          #+#    #+#             */
/*   Updated: 2026/05/12 18:29:43 by yel-azim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "codexion.h"

static int	check_number(long number, int i)
{
	if (i != 7 && number <= 0)
		return (printf("Error: argument %d must be > 0\n", i), 0);
	if (number < 0 || number > 2147483647)
		return (printf("Error: argument %d out of range\n", i), 0);
	return (1);
}

static int	check_schedular(char *data)
{
	if (strcmp(data, "fifo") == 0)
		return (1);
	if (strcmp(data, "edf") == 0)
		return (1);
	return (0);
}

static int	is_digit(char c)
{
	return (c >= '0' && c <= '9');
}

static int	ft_safe_atoi(char *str, long *result)
{
	int		i;
	long	num;
	int		digit;

	if (!str || str[0] == '\0')
		return (0);
	i = 0;
	while (str[i] == ' ' || str[i] == '\t')
		i++;
	if (str[i] == '+')
		i++;
	if (str[i] == '-' || str[i] == '\0')
		return (0);
	num = 0;
	while (str[i])
	{
		if (!is_digit(str[i]))
			return (0);
		digit = str[i] - '0';
		num = (num * 10) + digit;
		i++;
	}
	*result = num;
	return (1);
}

int	parse_data(char **data)
{
	int		i;
	long	number;

	i = 1;
	while (i <= 7)
	{
		if (!ft_safe_atoi(data[i], &number))
			return (printf("Error: invalid numeric argument '%s'\n",
					data[i]), 0);
		if (!check_number(number, i))
			return (0);
		i++;
	}
	if (!check_schedular(data[8]))
	{
		printf("Error: scheduler must be 'fifo' or 'edf'\n");
		return (0);
	}
	return (1);
}
