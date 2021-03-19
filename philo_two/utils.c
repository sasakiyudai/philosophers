/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: syudai <syudai@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 23:50:38 by syudai            #+#    #+#             */
/*   Updated: 2021/03/18 23:55:04 by syudai           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_two.h"

int		done_quota(t_info *ps)
{
	int ended;

	ended = 0;
	if (ps->must_eat > 0)
	{
		ps->must_eat--;
		if (ps->must_eat == 0)
		{
			sem_wait(g_quota);
			g_done++;
			if (g_done == ps->phil_count)
			{
				sem_wait(g_end);
				g_dead = 1;
				sem_post(g_end);
				ended = 1;
			}
			sem_post(g_quota);
		}
	}
	return (ended);
}

long	what_time(void)
{
	struct timeval	time_tv;

	gettimeofday(&time_tv, NULL);
	return (time_tv.tv_sec * 1000 + time_tv.tv_usec / 1000);
}

int		is_dead(t_info *ps, long eat_from)
{
	long	now;

	now = what_time();
	sem_wait(g_end);
	if (g_dead)
	{
		sem_post(g_end);
		return (1);
	}
	if (now - eat_from > ps->ms_die)
	{
		g_dead = 1;
		sem_post(g_end);
		sem_wait(g_print);
		printf("%ld %d died\n", what_time(), ps->id + 1);
		sem_post(g_print);
		return (1);
	}
	sem_post(g_end);
	return (0);
}

int		finish_something(long from, long work)
{
	long now;

	now = what_time();
	if (now - from >= work)
		return (1);
	return (0);
}

void	wait_destroy_and_free(int pc, pthread_t *threads, t_info *ps)
{
	int i;

	i = 0;
	while (i < pc)
		pthread_join(threads[i++], NULL);
	sem_close(g_mutex_fork);
	sem_close(g_mutex_only);
	sem_close(g_print);
	sem_close(g_end);
	sem_close(g_quota);
	sem_close(g_sem_get);
	sem_unlink("/g_mutex_fork");
	sem_unlink("/g_mutex_only");
	sem_unlink("/g_print");
	sem_unlink("/g_sem_get");
	sem_unlink("/g_end");
	sem_unlink("/g_quota");
	free(threads);
	free(ps);
}
