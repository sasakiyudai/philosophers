#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>


#define THINKING 0
#define HUNGRY 1
#define EATING 2

typedef struct {
  pthread_mutex_t *mon;
  pthread_cond_t **cv;
  int *state;
  int *tblock;     /* The is the time when the philosopher first blocks in pickup */
  int phil_count;
} Phil;

typedef struct {
  int id;                /* The philosopher's id: 0 to 5 */
  long t0;               /* The time when the program started */
  long ms;               /* The maximum time that philosopher sleeps/eats */
  void *v;               /* The void * that you define */
  long ms_eat;
  long ms_sleep;
  long ms_die;
  long must_eat;
  long *blocktime;      /* Total time that a philosopher is blocked */
  long *blockstarting;
  int phil_count;
  pthread_mutex_t *blockmon;   /* monitor for blocktime */             
} Phil_struct;

extern void *initialize_state(int phil_count);
extern void putdown(Phil_struct *);
void pickup(Phil_struct *ps, long eat_from);

int g_dead = 0;
int g_done = 0;
pthread_mutex_t g_print;
pthread_mutex_t g_end;
pthread_mutex_t g_quota;