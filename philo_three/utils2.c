/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: syudai <syudai@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 23:34:46 by syudai            #+#    #+#             */
/*   Updated: 2021/03/20 17:43:26 by syudai           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_three.h"

void	close_unlink_free(pthread_t *threads, t_info *ps, pid_t *pids)
{
	sem_close(g_mutex_fork);
	sem_close(g_mutex_only);
	sem_close(g_print);
	sem_close(g_sem_get);
	sem_unlink("/g_mutex_fork");
	sem_unlink("/g_mutex_only");
	sem_unlink("/g_print");
	sem_unlink("/g_sem_get");
	free(threads);
	free(ps);
	free(pids);
}

int		len_of_strs(char **strs)
{
	int i;

	i = 0;
	while (strs[i])
		i++;
	return (i);
}

int		wait_destroy_and_free(int pc, pid_t *pids, pthread_t *ths, t_info *ps)
{
	int i;
	int status;

	i = 0;
	while (i < pc)
	{
		waitpid(-1, &status, 0);
		if (WEXITSTATUS(status) == 1)
		{
			i = 0;
			while (i < pc)
				kill(pids[i++], SIGINT);
			break ;
		}
		i++;
	}
	close_unlink_free(ths, ps, pids);
	return (0);
}
