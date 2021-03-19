/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_three.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: syudai <syudai@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 22:19:40 by syudai            #+#    #+#             */
/*   Updated: 2021/03/18 23:36:38 by syudai           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_THREE_H
# define PHILO_THREE_H

# include <stdio.h>
# include <stdlib.h>
# include <signal.h>
# include <pthread.h>
# include <sys/time.h>
# include <unistd.h>
# include <semaphore.h>

extern int		g_get;
extern sem_t	*g_print;
extern sem_t	*g_mutex_fork;
extern sem_t	*g_mutex_only;
extern sem_t	*g_sem_get;

typedef struct	s_info
{
	int			id;
	long		ms_eat;
	long		ms_sleep;
	long		ms_die;
	long		must_eat;
	long		eat_from_instruct;
	int			phil_count;
}				t_info;

long			pickup(t_info *ps, long eat_from);
int				ft_atoi(const char *str);
int				init_ps(char **argv, int *phil_count,
pthread_t **threads, t_info **ps);
pid_t			process_create(t_info *ps);
int				finish_something(long from, long work);
long			what_time(void);
void			done_quota(t_info *ps);
long			pickup(t_info *ps, long eat_from);
void			is_dead(t_info *ps, long eat_from);
void			*philosopher(void *v);
int				wait_destroy_and_free(int pc,
pid_t *pids, pthread_t *ths, t_info *ps);
int				len_of_strs(char **strs);
void			close_unlink_free(pthread_t *threads, t_info *ps, pid_t *pids);

#endif
