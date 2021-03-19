/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: syudai <syudai@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 23:50:24 by syudai            #+#    #+#             */
/*   Updated: 2021/03/18 23:56:03 by syudai           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_two.h"

void	*dead_th_handler(void *ps)
{
	while (what_time() -
	((t_info *)ps)->eat_from_instruct <= ((t_info *)ps)->ms_die)
	{
		sem_wait(g_sem_get);
		sem_wait(g_end);
		if (g_get || g_dead)
		{
			sem_post(g_sem_get);
			sem_post(g_end);
			return (NULL);
		}
		sem_post(g_sem_get);
		sem_post(g_end);
		usleep(200);
	}
	sem_wait(g_end);
	if (!g_dead)
		printf("%ld %d died\n", what_time(), ((t_info *)ps)->id + 1);
	g_dead = 1;
	sem_post(g_end);
	return (NULL);
}

void	dead_th(t_info *ps, long eat_from)
{
	pthread_t tmp;

	ps->eat_from_instruct = eat_from;
	pthread_create(&tmp, NULL, dead_th_handler, (void *)ps);
	pthread_detach(tmp);
}

int		pick_one_fork(t_info *ps, long *eat_from)
{
	sem_wait(g_mutex_only);
	if (is_dead(ps, *eat_from))
	{
		sem_post(g_mutex_only);
		return (-1);
	}
	sem_wait(g_sem_get);
	g_get = 0;
	sem_post(g_sem_get);
	dead_th(ps, *eat_from);
	sem_wait(g_mutex_fork);
	if (is_dead(ps, *eat_from))
	{
		sem_post(g_mutex_only);
		sem_post(g_mutex_fork);
		return (-1);
	}
	return (0);
}

long	pickup(t_info *ps, long eat_from)
{
	long now;

	if (pick_one_fork(ps, &eat_from) < 0)
		return (-1);
	now = what_time();
	sem_wait(g_print);
	if (!g_dead)
		printf("%ld %d has taken a fork\n", now, ps->id + 1);
	sem_post(g_print);
	sem_wait(g_mutex_fork);
	sem_wait(g_sem_get);
	g_get = 1;
	sem_post(g_sem_get);
	sem_post(g_mutex_only);
	if (is_dead(ps, eat_from))
		return (-1);
	sem_wait(g_print);
	if (!g_dead)
		printf("%ld %d has taken a fork\n", now, ps->id + 1);
	sem_post(g_print);
	return (now);
}

long	p_eat(t_info *ps, long eat_from)
{
	if ((eat_from = pickup(ps, eat_from)) == -1)
		return (-1);
	sem_wait(g_print);
	if (!g_dead)
		printf("%ld %d is eating\n", eat_from, ps->id + 1);
	sem_post(g_print);
	while (!finish_something(eat_from, ps->ms_eat))
	{
		if (is_dead(ps, eat_from))
		{
			sem_post(g_mutex_fork);
			sem_post(g_mutex_fork);
			return (-1);
		}
		usleep(200);
	}
	sem_post(g_mutex_fork);
	sem_post(g_mutex_fork);
	if (done_quota(ps))
		return (-1);
	return (eat_from);
}
