#include "philo_one.h"

void *initialize_v(int phil_count);

long		what_time(void)
{
	long			time_ms;
	struct timeval	time_tv;

	gettimeofday(&time_tv, NULL);
	time_ms = time_tv.tv_sec * 1000;
	time_ms += time_tv.tv_usec / 1000;
	return (time_ms);
}

int	 is_dead(Phil_struct *ps, long eat_from)
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
		printf("%ld %d died\n", what_time(), ps->id);
		pthread_mutex_unlock(&g_print);
		return (1);
	}
	pthread_mutex_unlock(&g_end);
	return (0);
}

int finish_something(long from, long work)
{
	long now;
	
	now = what_time();
	if (now - from >= work)
		return (1);
	return (0);
}

int can_I_eat(Phil *pp, int n)
{
  long t;
  int phil_count;
  
  phil_count = pp->phil_count;

  if (pp->state[(n+(phil_count-1))%phil_count] == EATING ||
      pp->state[(n+1)%phil_count] == EATING) return 0;
  t = what_time();
  if (pp->state[(n+(phil_count-1))%phil_count] == HUNGRY &&
      pp->tblock[(n+(phil_count-1))%phil_count] < pp->tblock[n]) return 0;
  if (pp->state[(n+1)%phil_count] == HUNGRY &&
      pp->tblock[(n+1)%phil_count] < pp->tblock[n]) return 0;
  return 1;
}

void pickup(Phil_struct *ps, long eat_from)
{
  Phil *pp;

  pp = (Phil *) ps->v;
  
  pthread_mutex_lock(pp->mon);
  pp->state[ps->id] = HUNGRY;
  pp->tblock[ps->id] = what_time();
  while (!can_I_eat(pp, ps->id)) {
	  if (is_dead(ps, eat_from))
		{
			exit(0);
		}
    pthread_cond_wait(pp->cv[ps->id], pp->mon);
	usleep(200);
  }
  pthread_mutex_lock(&g_print);
  if (!g_dead)
  {
	printf("%ld %d has taken a fork\n", 
					what_time(), ps->id);
	printf("%ld %d has taken a fork\n", 
					what_time(), ps->id);
  }
  pthread_mutex_unlock(&g_print);
  pp->state[ps->id] = EATING;
  pthread_mutex_unlock(pp->mon);
}

void putdown(Phil_struct *ps)
{
  Phil *pp;
  int phil_count;

  pp = (Phil *) ps->v;
  phil_count = pp->phil_count;

  pthread_mutex_lock(pp->mon);
  pp->state[ps->id] = THINKING;
  if (pp->state[(ps->id+(phil_count-1))%phil_count] == HUNGRY) {
    pthread_cond_signal(pp->cv[(ps->id+(phil_count-1))%phil_count]);
  }
  if (pp->state[(ps->id+1)%phil_count] == HUNGRY) {
    pthread_cond_signal(pp->cv[(ps->id+1)%phil_count]);
  }
  pthread_mutex_unlock(pp->mon);
}

void *initialize_v(int phil_count)
{
  Phil *pp;
  int i;

  pp = (Phil *) malloc(sizeof(Phil));
  pp->phil_count = phil_count;
  pp->mon = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pp->cv = (pthread_cond_t **) malloc(sizeof(pthread_cond_t *)*phil_count);
  if (pp->cv == NULL) { perror("malloc"); exit(1); }
  pp->state = (int *) malloc(sizeof(int)*phil_count);
  if (pp->state == NULL) { perror("malloc"); exit(1); }
  pp->tblock = (int *) malloc(sizeof(int)*phil_count);
  if (pp->tblock == NULL) { perror("malloc"); exit(1); }
  pthread_mutex_init(pp->mon, NULL);
  for (i = 0; i < phil_count; i++) {
    pp->cv[i] = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
    pthread_cond_init(pp->cv[i], NULL);
    pp->state[i] = THINKING;
    pp->tblock[i] = 0;
  }

  return (void *) pp;
}

long p_eat(Phil_struct *ps, long eat_from)
{
	  long t;

	 /* He calls pickup to pick up the chopsticks */

    t = what_time();
    pthread_mutex_lock(ps->blockmon);
    ps->blockstarting[ps->id] = t;
    pthread_mutex_unlock(ps->blockmon);

    pickup(ps, eat_from);

    pthread_mutex_lock(ps->blockmon);
	eat_from = what_time();
    ps->blocktime[ps->id] += (eat_from - t);
    ps->blockstarting[ps->id] = -1;
    pthread_mutex_unlock(ps->blockmon);

    /* When pickup returns, the philosopher can eat for a random number of
       seconds */
	pthread_mutex_lock(&g_print);
	if (!g_dead)
    	printf("%ld %d is eating\n", what_time(), ps->id);
	pthread_mutex_unlock(&g_print);
    while (!finish_something(eat_from, ps->ms_eat))
	{
		if (is_dead(ps, eat_from))
		{
			exit(0);
		}
		usleep(200);
	}
	putdown(ps);
	return eat_from;
}

int done_quota(Phil_struct *ps)
{
  int ended = 0;

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

void p_sleep(Phil_struct *ps, long eat_from)
{
	long sleep_from;

	sleep_from = what_time();
	pthread_mutex_lock(&g_print);
	if (!g_dead)
    	printf("%ld %d is sleeping\n",
                what_time(), ps->id);
    pthread_mutex_unlock(&g_print);
    
	while (!finish_something(sleep_from, ps->ms_sleep))
	{
		if (is_dead(ps, eat_from))
		{
			exit(0);
		}
		usleep(200);
	}
  if (done_quota(ps))
    exit(0);
}

void *philosopher(void *v)
{
  Phil_struct *ps;
  long st;
  long t;
  long eat_from;

  eat_from = what_time();
  ps = (Phil_struct *) v;


  while(1) {

	eat_from = p_eat(ps, eat_from);

	p_sleep(ps, eat_from);

    /* think */
	pthread_mutex_lock(&g_print);
	if (!g_dead)
    	printf("%ld %d is thinking\n", 
                what_time(), ps->id);
    pthread_mutex_unlock(&g_print);

  }
}

int main(int argc, char **argv)
{
  int i;
  pthread_t *threads;
  Phil_struct *ps;
  void *v;
  long t0, ttmp, ttmp2;
  pthread_mutex_t *blockmon;
  long *blocktime;
  long *blockstarting;
  char s[500];
  int phil_count;
  char *curr;
  long total;

  if (argc != 5 && argc != 6) {
    fprintf(stderr, "usage: dphil philosopher_count maxsleepsec\n");
    exit(1);
  }

  phil_count = atoi(argv[1]);
  threads = (pthread_t *) malloc(sizeof(pthread_t)*phil_count);
  if (threads == NULL) { perror("malloc"); exit(1); }
  ps = (Phil_struct *) malloc(sizeof(Phil_struct)*phil_count);
  if (ps == NULL) { perror("malloc"); exit(1); }
  
   
  v = initialize_v(phil_count);
  t0 = what_time();
  blocktime = (long *) malloc(sizeof(long)*phil_count);
  if (blocktime == NULL) { perror("malloc blocktime"); exit(1); }
  blockstarting = (long *) malloc(sizeof(long)*phil_count);
  if (blockstarting == NULL) { perror("malloc blockstarting"); exit(1); }

  blockmon = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(blockmon, NULL);
  for (i = 0; i < phil_count; i++) {
    blocktime[i] = 0;
    blockstarting[i] = -1;
  }
  pthread_mutex_init(&g_end, NULL);
  pthread_mutex_init(&g_print, NULL);
  pthread_mutex_init(&g_quota, NULL);

  for (i = 0; i < phil_count; i++) {
    ps[i].id = i;
    ps[i].t0 = t0;
    ps[i].v = v;
    ps[i].ms_die = atoi(argv[2]);
    ps[i].ms_eat = atoi(argv[3]);
    ps[i].ms_sleep = atoi(argv[4]);
    if (argc == 6)
      ps[i].must_eat = atoi(argv[5]);
    else
      ps[i].must_eat = -1;
    ps[i].blocktime = blocktime;
    ps[i].blockstarting = blockstarting;
    ps[i].blockmon = blockmon;
    ps[i].phil_count = phil_count;
    pthread_create(threads+i, NULL, philosopher, (void *) (ps+i));
  }

  i = 0;
while (i < phil_count)
	pthread_join(threads[i++], NULL);
}
