/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: syudai <syudai@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 22:12:46 by syudai            #+#    #+#             */
/*   Updated: 2021/03/25 12:38:16 by syudai           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_two.h"

int		g_dead = 0;
int		g_done = 0;
int		g_get = 0;
sem_t	*g_print;
sem_t	*g_end;
sem_t	*g_quota;
sem_t	*g_mutex_fork;
sem_t	*g_mutex_only;
sem_t	*g_sem_get;

long	p_sleep(t_info *ps, long eat_from)
{
	long sleep_from;

	sleep_from = what_time();
	sem_wait(g_print);
	if (!g_dead)
		printf("%ld %d is sleeping\n", what_time(), ps->id + 1);
	sem_post(g_print);
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
	while (1)
	{
		if ((eat_from = p_eat(ps, eat_from)) == -1)
			return (NULL);
		if ((p_sleep(ps, eat_from)) == -1)
			return (NULL);
		sem_wait(g_print);
		if (!g_dead)
			printf("%ld %d is thinking\n", what_time(), ps->id + 1);
		sem_post(g_print);
	}
}

int		init_ps(char **argv, int *phil_count, pthread_t **threads, t_info **ps)
{
	if ((*phil_count = ft_atoi(argv[1])) < 2)
		return (-1);
	if (!(*threads = (pthread_t *)malloc(sizeof(pthread_t) * (*phil_count))))
		return (-1);
	if (!(*ps = (t_info *)malloc(sizeof(t_info) * (*phil_count))))
		return (-1);
	sem_unlink("/g_mutex_fork");
	sem_unlink("/g_mutex_only");
	sem_unlink("/g_print");
	sem_unlink("/g_sem_get");
	sem_unlink("/g_end");
	sem_unlink("/g_quota");
	g_mutex_fork = sem_open("/g_mutex_fork", O_CREAT, 0777, *phil_count);
	g_mutex_only = sem_open("/g_mutex_only", O_CREAT, 0777, 1);
	g_print = sem_open("/g_print", O_CREAT, 0777, 1);
	g_end = sem_open("/g_end", O_CREAT, 0777, 1);
	g_quota = sem_open("/g_quota", O_CREAT, 0777, 1);
	g_sem_get = sem_open("/g_sem_get", O_CREAT, 0777, 1);
	return (0);
}

int		main(int a, char **argv)
{
	int			i;
	int			phil_count;
	pthread_t	*threads;
	t_info		*ps;

	if ((a != 5 && a != 6) || (init_ps(argv, &phil_count, &threads, &ps)) < 0)
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
