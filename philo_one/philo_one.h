/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_one.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: syudai <syudai@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/15 22:28:33 by syudai            #+#    #+#             */
/*   Updated: 2021/03/18 10:04:14 by syudai           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_ONE_H
# define PHILO_ONE_H

# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <sys/time.h>
# include <unistd.h>

extern int				g_dead;
extern int				g_done;
int						*g_fork;
int						*g_yoyaku;
extern pthread_mutex_t	g_print;
extern pthread_mutex_t	g_end;
extern pthread_mutex_t	g_quota;
extern pthread_mutex_t	g_mutex_done;
extern pthread_mutex_t	g_mutex_fork;

typedef struct			s_info
{
	int					id;
	void				*v;
	long				ms_eat;
	long				ms_sleep;
	long				ms_die;
	long				must_eat;
	int					phil_count;
}						t_info;

long					pickup(t_info *ps, long eat_from);
int						ft_atoi(const char *str);
long					what_time(void);
int						finish_something(long from, long work);
void					wait_destroy_and_free(int phil_count,
pthread_t *threads, t_info *ps);
int						done_quota(t_info *ps);
int						init_ps(char **argv, int *phil_count,
pthread_t **threads, t_info **ps);
long					pickup(t_info *ps, long eat_from);
int						can_i_eat(t_info *ps);
int						can_i_have(t_info *ps);
int						is_dead(t_info *ps, long eat_from);

#endif
