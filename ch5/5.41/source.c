#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <limits.h>
#include "../../lib/parser.h"

#define STR_SIZE 100

static int barrier_threshold;

static pthread_mutex_t barrier_mtx;
static sem_t barrier_next, barrier_x_sem;
static int barrier_next_count = 0, barrier_x_count = 0;

struct condition {
  int x_count;
  sem_t x_sem;
};

static pthread_mutex_t mon_barrier_mtx;
static sem_t mon_barrier_next;
static int mon_barrier_next_count = 0;
struct condition mon_x;

void condition_init(struct condition*);
void condition_wait(struct condition*);
void condition_signal(struct condition*);

bool are_params_valid(int, int);
int init(int);
int barrier_point();
int monitor_barrier_point();
void* runner(void*);
void* monitor_runner(void*);

static pthread_mutex_t print_mtx;
static int  _total, _batch, _count, _round;

int main(int argc, char** argv){
  char str[STR_SIZE] = { 0 }, c;
  pthread_t t_tid, *tid_pool;
  pthread_attr_t attr;

  printf("Run Barrier Point!\n");
  pthread_mutex_init(&print_mtx, NULL);

  pthread_mutex_init(&barrier_mtx, NULL);
  sem_init(&barrier_next, 0, 0);
  sem_init(&barrier_x_sem, 0, 0);

  pthread_mutex_init(&mon_barrier_mtx, NULL);
  sem_init(&mon_barrier_next, 0, 0);
  condition_init(&mon_x);

  do{
    printf("Please choose type of runner - monitor runner (m/M), normal runner (n/N):");
    fgets(str, STR_SIZE, stdin);
    c = str[0];
  }while(c != 'm' && c != 'M' && c != 'n' && c != 'N');
  do{
    printf("Please type number of batches of threads to run: ");
    fgets(str, STR_SIZE, stdin);
    int len = strlen(str);
    if(len > 0){
      if(str[len-1] == '\n')
        str[len-1] = '\0';
    }
    _batch = atoi(str);
  }while(!is_integer(str) || _batch <= 0);
  do{
    printf("Please type the barrier point: ");
    fgets(str, STR_SIZE, stdin);
    int len = strlen(str);
    if(len > 0){
      if(str[len-1] == '\n')
        str[len-1] = '\0';
    }
    _count = atoi(str);
  }while(!is_integer(str) || !are_params_valid(_batch, _count));

  _total = _batch * _count;
  _round = 0;
  tid_pool = (pthread_t*)malloc(_total * sizeof(pthread_t));
  init(_count);

  pthread_attr_init(&attr);
  for(int i = 0; i < _total; i++){
    if(c == 'm' || c == 'M')
      pthread_create(&t_tid, &attr, monitor_runner, NULL);
    else
      pthread_create(&t_tid, &attr, runner, NULL);

    tid_pool[i] = t_tid;
    pthread_mutex_lock(&print_mtx);
    printf("Main, i=%d, create thread %lu\n", i, t_tid);
    pthread_mutex_unlock(&print_mtx);
  }
  for(int i = 0; i < _total; i++)
    pthread_join(tid_pool[i], NULL);

  printf("Finished pthreads = %d, total = %d\n", _round, _total);

  return 0;
}

int init(int N){
  if(N <= 0) return -1;
  barrier_threshold = N;
  return 0;
}

int barrier_point(){
  int ret = 0;
  unsigned long pid = pthread_self();
  pthread_mutex_lock(&barrier_mtx);
  if(barrier_threshold <= 0 || barrier_x_count < 0){
    ret = -1;
    goto EXIT;
  }
  if(barrier_x_count == 0) _round++;
  pthread_mutex_lock(&print_mtx);
  printf("Runner %lu, round %d, >>>>>>enter barrier point, barrier_threshold=%d, barrier_x_count=%d>>>>>>\n", pid, _round, barrier_threshold, barrier_x_count);
  pthread_mutex_unlock(&print_mtx);
  
  barrier_x_count++;
  if(barrier_x_count < barrier_threshold){
    pthread_mutex_unlock(&barrier_mtx);
    sem_wait(&barrier_x_sem);
    barrier_x_count--;
  }
  else{
    barrier_x_count--;
    for(;barrier_x_count > 0;){
      barrier_next_count++;
      sem_post(&barrier_x_sem);
      sem_wait(&barrier_next);
      barrier_next_count--;
    }
  }
EXIT:
  pthread_mutex_lock(&print_mtx);
  printf("Runner %lu, round %d, ======leave barrier point, barrier_threshold=%d, barrier_x_count=%d======\n", pid, _round, barrier_threshold, barrier_x_count);
  pthread_mutex_unlock(&print_mtx);
 
  if(barrier_next_count > 0)
    sem_post(&barrier_next);
  else
    pthread_mutex_unlock(&barrier_mtx);
  return ret;
}

int monitor_barrier_point(){
  int ret = 0;
  unsigned long pid = pthread_self();
  pthread_mutex_lock(&mon_barrier_mtx);
  if(barrier_threshold <= 0 || mon_x.x_count < 0){
    ret = -1;
    goto EXIT;
  }
  if(mon_x.x_count == 0) _round++;
  pthread_mutex_lock(&print_mtx);
  printf("Runner %lu, round %d, >>>>>>enter barrier point, barrier_threshold=%d, mon_x.x_count=%d>>>>>>\n", pid, _round, barrier_threshold, mon_x.x_count);
  pthread_mutex_unlock(&print_mtx);

  if(mon_x.x_count < barrier_threshold - 1){
    condition_wait(&mon_x);
  }
  else{
    for(;mon_x.x_count > 0;){
      condition_signal(&mon_x);
    }
  }
EXIT:
  pthread_mutex_lock(&print_mtx);
  printf("Runner %lu, round %d, ======leave barrier point, barrier_threshold=%d, mon_x.x_count=%d======\n", pid, _round, barrier_threshold, mon_x.x_count);
  pthread_mutex_unlock(&print_mtx);

  if(mon_barrier_next_count > 0)
    sem_post(&mon_barrier_next);
  else
    pthread_mutex_unlock(&mon_barrier_mtx);
  return ret;
}

void* runner(void* param){
  if(barrier_point()){
    pthread_mutex_lock(&print_mtx);
    printf("Runner %lu, error barrier point\n", pthread_self());
    pthread_mutex_unlock(&print_mtx);
  }
  pthread_exit(0);
}

void* monitor_runner(void* param){
  if(monitor_barrier_point()){
    pthread_mutex_lock(&print_mtx);
    printf("Runner %lu, error barrier point\n", pthread_self());
    pthread_mutex_unlock(&print_mtx);
  }
  pthread_exit(0);
}

bool are_params_valid(int cycles, int count){
  if(cycles <= 0 || count <= 0) return false;
  int acum = 0;
  for(int i = 0; i < cycles; i++){
    if(acum > INT_MAX - count) return false;
    acum += count;
  }
  return true;
}

void condition_init(struct condition* x){
  x->x_count = 0;
  sem_init(&x->x_sem, 0, 0);
}

void condition_wait(struct condition* x){
  x->x_count++;
  if(mon_barrier_next_count > 0)
    sem_post(&mon_barrier_next);
  else
    pthread_mutex_unlock(&mon_barrier_mtx);
  sem_wait(&x->x_sem);
  x->x_count--;
}

void condition_signal(struct condition* x){
  if(x->x_count > 0){
    mon_barrier_next_count++;
    sem_post(&x->x_sem);
    sem_wait(&mon_barrier_next);
    mon_barrier_next_count--;
  }
}

