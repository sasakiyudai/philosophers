/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_two.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: syudai <syudai@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/15 23:38:12 by syudai            #+#    #+#             */
/*   Updated: 2021/03/18 23:55:20 by syudai           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_TWO_H
# define PHILO_TWO_H

# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <sys/time.h>
# include <unistd.h>
# include <semaphore.h>

extern int		g_dead;
extern int		g_done;
extern int		g_get;
extern sem_t	*g_print;
extern sem_t	*g_end;
extern sem_t	*g_quota;
extern sem_t	*g_mutex_fork;
extern sem_t	*g_mutex_only;
extern sem_t	*g_sem_get;

typedef struct	s_info
{
	int			id;
	int			phil_count;
	long		ms_eat;
	long		ms_sleep;
	long		ms_die;
	long		must_eat;
	long		eat_from_instruct;
}				t_info;

long			pickup(t_info *ps, long eat_from);
int				ft_atoi(const char *str);
void			wait_destroy_and_free(int pc, pthread_t *threads, t_info *ps);
int				finish_something(long from, long work);
int				is_dead(t_info *ps, long eat_from);
long			what_time(void);
int				done_quota(t_info *ps);
long			p_eat(t_info *ps, long eat_from);
void			dead_th(t_info *ps, long eat_from);

#endif
