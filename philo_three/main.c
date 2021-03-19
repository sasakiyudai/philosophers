/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: syudai <syudai@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 22:32:52 by syudai            #+#    #+#             */
/*   Updated: 2021/03/18 23:36:51 by syudai           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_three.h"

int		g_get = 0;
sem_t	*g_print;
sem_t	*g_mutex_fork;
sem_t	*g_mutex_only;
sem_t	*g_sem_get;

long	p_eat(t_info *ps, long eat_from)
{
	eat_from = pickup(ps, eat_from);
	sem_wait(g_print);
	printf("%ld %d is eating\n", eat_from, ps->id + 1);
	sem_post(g_print);
	while (!finish_something(eat_from, ps->ms_eat))
	{
		is_dead(ps, eat_from);
		usleep(200);
	}
	sem_post(g_mutex_fork);
	sem_post(g_mutex_fork);
	done_quota(ps);
	return (eat_from);
}

void	p_sleep(t_info *ps, long eat_from)
{
	long sleep_from;

	sleep_from = what_time();
	sem_wait(g_print);
	printf("%ld %d is sleeping\n", what_time(), ps->id + 1);
	sem_post(g_print);
	while (!finish_something(sleep_from, ps->ms_sleep))
	{
		is_dead(ps, eat_from);
		usleep(200);
	}
}

void	*philosopher(void *v)
{
	t_info	*ps;
	long	eat_from;

	eat_from = what_time();
	ps = (t_info *)v;
	while (1)
	{
		eat_from = p_eat(ps, eat_from);
		p_sleep(ps, eat_from);
		sem_wait(g_print);
		printf("%ld %d is thinking\n", what_time(), ps->id + 1);
		sem_post(g_print);
	}
}

void	init_ps_and_make_process(t_info *ps, char **argv, int pc, pid_t *pids)
{
	int i;
	int len_argv;

	i = -1;
	len_argv = len_of_strs(argv);
	while (++i < pc)
	{
		ps[i].id = i;
		ps[i].ms_die = ft_atoi(argv[2]);
		ps[i].ms_eat = ft_atoi(argv[3]);
		ps[i].ms_sleep = ft_atoi(argv[4]);
		if (len_argv == 6)
			ps[i].must_eat = ft_atoi(argv[5]);
		else
			ps[i].must_eat = -1;
		ps[i].phil_count = pc;
		pids[i] = process_create(ps + i);
	}
}

int		main(int a, char **argv)
{
	pthread_t	*threads;
	t_info		*ps;
	pid_t		*pids;
	int			phil_count;

	if ((a != 5 && a != 6) || (init_ps(argv, &phil_count, &threads, &ps)) < 0
	|| !(pids = malloc(sizeof(pid_t) * phil_count)))
	{
		printf("Error\n");
		return (1);
	}
	init_ps_and_make_process(ps, argv, phil_count, pids);
	wait_destroy_and_free(phil_count, pids, threads, ps);
}
