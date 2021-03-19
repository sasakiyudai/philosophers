/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   eat_help.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: syudai <syudai@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 10:29:15 by syudai            #+#    #+#             */
/*   Updated: 2021/03/18 10:30:18 by syudai           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_three.h"

void	is_dead(t_info *ps, long eat_from)
{
	long	now;

	now = what_time();
	if (now - eat_from > ps->ms_die)
	{
		sem_wait(g_print);
		printf("%ld %d died\n", what_time(), ps->id + 1);
		sem_post(g_print);
		exit(1);
	}
}

void	*dead_th_handler(void *ps)
{
	while (what_time() -
	((t_info *)ps)->eat_from_instruct <= ((t_info *)ps)->ms_die)
	{
		sem_wait(g_sem_get);
		if (g_get)
		{
			sem_post(g_sem_get);
			return (NULL);
		}
		sem_post(g_sem_get);
		usleep(200);
	}
	sem_wait(g_print);
	printf("%ld %d died\n", what_time(), ((t_info *)ps)->id + 1);
	sem_post(g_print);
	exit(1);
	return (NULL);
}

void	dead_th(t_info *ps, long eat_from)
{
	pthread_t tmp;

	ps->eat_from_instruct = eat_from;
	pthread_create(&tmp, NULL, dead_th_handler, (void *)ps);
	pthread_detach(tmp);
}

long	pickup(t_info *ps, long eat_from)
{
	long now;

	sem_wait(g_mutex_only);
	g_get = 0;
	dead_th(ps, eat_from);
	sem_wait(g_mutex_fork);
	now = what_time();
	sem_wait(g_print);
	printf("%ld %d has taken a fork\n", now, ps->id + 1);
	sem_post(g_print);
	sem_wait(g_mutex_fork);
	sem_wait(g_sem_get);
	g_get = 1;
	sem_post(g_sem_get);
	sem_post(g_mutex_only);
	sem_wait(g_print);
	printf("%ld %d has taken a fork\n", now, ps->id + 1);
	sem_post(g_print);
	return (now);
}
