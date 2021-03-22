/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: syudai <syudai@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 10:23:16 by syudai            #+#    #+#             */
/*   Updated: 2021/03/20 17:37:59 by syudai           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_three.h"

void	done_quota(t_info *ps)
{
	if (ps->must_eat > 0)
	{
		ps->must_eat--;
		if (ps->must_eat == 0)
			exit(0);
	}
}

long	what_time(void)
{
	struct timeval	time_tv;

	gettimeofday(&time_tv, NULL);
	return (time_tv.tv_sec * 1000 + time_tv.tv_usec / 1000);
}

int		finish_something(long from, long work)
{
	long now;

	now = what_time();
	if (now - from >= work)
		return (1);
	return (0);
}

pid_t	process_create(t_info *ps)
{
	pid_t ret;

	if ((ret = fork()) < 0)
		return (-1);
	else if (ret == 0)
	{
		philosopher(ps);
		return (-1);
	}
	else
		return (ret);
}

int		init_ps(char **argv, int *phil_count, pthread_t **threads, t_info **ps)
{
	*phil_count = ft_atoi(argv[1]);
	if (!(*threads = (pthread_t *)malloc(sizeof(pthread_t) * *phil_count)))
		return (-1);
	if (!(*ps = (t_info *)malloc(sizeof(t_info) * *phil_count)))
		return (-1);
	sem_unlink("/g_mutex_fork");
	sem_unlink("/g_mutex_only");
	sem_unlink("/g_print");
	sem_unlink("/g_sem_get");
	g_mutex_fork = sem_open("/g_mutex_fork", O_CREAT, 0777, *phil_count);
	g_mutex_only = sem_open("/g_mutex_only", O_CREAT, 0777, 1);
	g_print = sem_open("/g_print", O_CREAT, 0777, 1);
	g_sem_get = sem_open("/g_sem_get", O_CREAT, 0777, 1);
	return (0);
}
