/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   eat_help.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: syudai <syudai@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 09:59:19 by syudai            #+#    #+#             */
/*   Updated: 2021/03/21 09:31:48 by syudai           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_one.h"

int		can_i_have(t_info *ps)
{
	if (!g_fork[ps->id] && !g_fork[(ps->id + 1) % ps->phil_count]
		&& (g_yoyaku[ps->id] == ps->id || g_yoyaku[ps->id] == -1)
		&& (g_yoyaku[(ps->id + 1) % ps->phil_count] == ps->id
			|| g_yoyaku[(ps->id + 1) % ps->phil_count] == -1))
		return (1);
	if (g_yoyaku[ps->id] == -1)
		g_yoyaku[ps->id] = ps->id;
	if (g_yoyaku[(ps->id + 1) % ps->phil_count] == -1)
		g_yoyaku[(ps->id + 1) % ps->phil_count] = ps->id;
	return (0);
}

int		can_i_eat(t_info *ps)
{
	long	t;
	int		phil_count;

	phil_count = ps->phil_count;
	pthread_mutex_lock(&g_mutex_forks[ps->id]);
	pthread_mutex_lock(&g_mutex_forks[(ps->id + 1) % phil_count]);
	if (can_i_have(ps))
	{
		t = what_time();
		g_fork[ps->id] = 1;
		if (!g_dead)
			printf("%ld %d has taken a fork\n", what_time(), ps->id + 1);
		g_yoyaku[ps->id] = -1;
		g_fork[(ps->id + 1) % phil_count] = 1;
		if (!g_dead)
			printf("%ld %d has taken a fork\n", what_time(), ps->id + 1);
		g_yoyaku[(ps->id + 1) % phil_count] = -1;
		pthread_mutex_unlock(&g_mutex_forks[ps->id]);
		pthread_mutex_unlock(&g_mutex_forks[(ps->id + 1) % phil_count]);
		return (1);
	}
	pthread_mutex_unlock(&g_mutex_forks[ps->id]);
	pthread_mutex_unlock(&g_mutex_forks[(ps->id + 1) % phil_count]);
	return (0);
}

long	pickup(t_info *ps, long eat_from)
{
	while (!can_i_eat(ps))
	{
		if (is_dead(ps, eat_from))
			return (-1);
		usleep(200);
	}
	return (0);
}
