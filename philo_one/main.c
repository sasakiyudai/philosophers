/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: syudai <syudai@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/15 22:36:09 by syudai            #+#    #+#             */
/*   Updated: 2021/03/21 09:29:01 by syudai           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_one.h"

int				g_dead = 0;
int				g_done = 0;
int				*g_fork;
int				*g_yoyaku;
pthread_mutex_t g_print;
pthread_mutex_t g_end;
pthread_mutex_t g_quota;
pthread_mutex_t g_mutex_done;
pthread_mutex_t *g_mutex_forks;

int		is_dead(t_info *ps, long eat_from)
{
	long	now;

	now = what_time();
	pthread_mutex_lock(&g_end);
	if (g_dead)
	{
		pthread_mutex_unlock(&g_end);
		return (1);
	}
	if (now - eat_from > ps->ms_die)
	{
		g_dead = 1;
		pthread_mutex_unlock(&g_end);
		pthread_mutex_lock(&g_print);
		printf("%ld %d died\n", what_time(), ps->id + 1);
		pthread_mutex_unlock(&g_print);
		return (1);
	}
	pthread_mutex_unlock(&g_end);
	return (0);
}

long	p_eat(t_info *ps, long eat_from)
{
	if (pickup(ps, eat_from) == -1)
		return (-1);
	pthread_mutex_lock(&g_print);
	eat_from = what_time();
	if (!g_dead)
		printf("%ld %d is eating\n", what_time(), ps->id + 1);
	pthread_mutex_unlock(&g_print);
	while (!finish_something(eat_from, ps->ms_eat))
	{
		if (is_dead(ps, eat_from))
			return (-1);
		usleep(200);
	}
	if (done_quota(ps))
		return (-1);
	return (eat_from);
}

long	p_sleep(t_info *ps, long eat_from)
{
	long sleep_from;

	sleep_from = what_time();
	pthread_mutex_lock(&g_print);
	if (!g_dead)
		printf("%ld %d is sleeping\n", what_time(), ps->id + 1);
	pthread_mutex_unlock(&g_print);
	while (!finish_something(sleep_from, ps->ms_sleep))
	{
		if (is_dead(ps, eat_from))
			return (-1);
		usleep(200);
	}
	return (0);
}

void	*philosopher(void *v)
{
	t_info	*ps;
	long	eat_from;

	eat_from = what_time();
	ps = (t_info *)v;
	if (ps->id % 2)
		usleep(200);
	while (1)
	{
		if ((eat_from = p_eat(ps, eat_from)) == -1)
			return (NULL);
		if (p_sleep(ps, eat_from) == -1)
			return (NULL);
		pthread_mutex_lock(&g_print);
		if (!g_dead)
			printf("%ld %d is thinking\n", what_time(), ps->id + 1);
		pthread_mutex_unlock(&g_print);
	}
}

int		main(int a, char **argv)
{
	int			i;
	int			phil_count;
	pthread_t	*threads;
	t_info		*ps;

	if ((a != 5 && a != 6) || (init_ps(argv, &phil_count, &threads, &ps) < 0))
	{
		printf("Error\n");
		return (1);
	}
	i = -1;
	while (++i < phil_count)
	{
		ps[i].id = i;
		ps[i].ms_die = ft_atoi(argv[2]);
		ps[i].ms_eat = ft_atoi(argv[3]);
		ps[i].ms_sleep = ft_atoi(argv[4]);
		if (a == 6)
			ps[i].must_eat = ft_atoi(argv[5]);
		else
			ps[i].must_eat = -1;
		ps[i].phil_count = phil_count;
		pthread_create(threads + i, NULL, philosopher, (void *)(ps + i));
	}
	wait_destroy_and_free(phil_count, threads, ps);
}
