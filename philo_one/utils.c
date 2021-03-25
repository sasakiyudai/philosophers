/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: syudai <syudai@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 09:39:37 by syudai            #+#    #+#             */
/*   Updated: 2021/03/25 12:41:39 by syudai           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_one.h"

long	what_time(void)
{
	struct timeval	time_tv;

	gettimeofday(&time_tv, NULL);
	return (time_tv.tv_sec * 1000 + time_tv.tv_usec / 1000);
}

int		finish_something(long from, long work)
{
	if (what_time() - from >= work)
		return (1);
	return (0);
}

void	wait_destroy_and_free(int phil_count, pthread_t *threads, t_info *ps)
{
	int i;

	i = 0;
	while (i < phil_count)
		pthread_join(threads[i++], NULL);
	i = 0;
	while (i < phil_count)
		pthread_mutex_destroy(&g_mutex_forks[i++]);
	pthread_mutex_destroy(&g_print);
	pthread_mutex_destroy(&g_end);
	pthread_mutex_destroy(&g_quota);
	pthread_mutex_destroy(&g_mutex_done);
	free(threads);
	free(ps);
	free(g_fork);
	free(g_yoyaku);
}

int		done_quota(t_info *ps)
{
	int ended;

	ended = 0;
	pthread_mutex_lock(&g_mutex_done);
	g_fork[ps->id] = 0;
	g_fork[(ps->id + 1) % ps->phil_count] = 0;
	pthread_mutex_unlock(&g_mutex_done);
	if (ps->must_eat > 0)
	{
		ps->must_eat--;
		if (ps->must_eat == 0)
		{
			pthread_mutex_lock(&g_quota);
			g_done++;
			if (g_done == ps->phil_count)
			{
				pthread_mutex_lock(&g_end);
				g_dead = 1;
				pthread_mutex_unlock(&g_end);
				ended = 1;
			}
			pthread_mutex_unlock(&g_quota);
		}
	}
	return (ended);
}

int		init_ps(char **argv, int *pc, pthread_t **threads, t_info **ps)
{
	int i;

	if ((*pc = ft_atoi(argv[1])) < 2)
		return (-1);
	if (!(*threads = (pthread_t *)malloc(sizeof(pthread_t) * (*pc))))
		return (-1);
	if (!(*ps = (t_info *)malloc(sizeof(t_info) * (*pc))))
		return (-1);
	if (!(g_yoyaku = (int *)malloc(sizeof(int) * (*pc)))
	|| !(g_fork = (int *)malloc(sizeof(int) * (*pc))))
		return (-1);
	if (!(g_mutex_forks = malloc(sizeof(pthread_mutex_t) * *pc)))
		return (-1);
	i = 0;
	while (i < *pc)
	{
		g_fork[i] = 0;
		g_yoyaku[i++] = -1;
		pthread_mutex_init(&g_mutex_forks[i], NULL);
	}
	pthread_mutex_init(&g_end, NULL);
	pthread_mutex_init(&g_print, NULL);
	pthread_mutex_init(&g_quota, NULL);
	pthread_mutex_init(&g_mutex_done, NULL);
	return (0);
}
