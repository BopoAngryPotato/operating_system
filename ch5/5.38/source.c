#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#define STR_SIZE 100
#define MAX_RESOURCES 5
#define CYCLE 20

struct condition {
  int x_count;
  sem_t x_sem;
};

void condition_init(struct condition*);
void condition_wait(struct condition*);
void condition_signal(struct condition*);
bool condition_check_waiting(int);
struct condition* condition_select_waiting(int);

pthread_mutex_t resources_mtx;
sem_t resources_next;
int resources_next_count = 0;
struct condition x[MAX_RESOURCES];

void decrease_count(int);
void increase_count(int);

void* monitor(void*);
void* runner(void*);

int available_resources = MAX_RESOURCES, obt = 0, rel = 0;
pthread_mutex_t obt_mtx, rel_mtx, print_mtx;
bool mc = true;
int params[CYCLE];

int main(){
  char str[STR_SIZE] = { 0 };
  pthread_t monitor_tid, t_tid;
  pthread_attr_t attr;
  printf("Run resource allocator!\n");
  pthread_mutex_init(&resources_mtx, NULL);
  sem_init(&resources_next, 0, 0);
  for(int i = 0; i < MAX_RESOURCES; i++)
    condition_init(&x[i]);

  pthread_mutex_init(&obt_mtx, NULL);
  pthread_mutex_init(&rel_mtx, NULL);
  pthread_mutex_init(&print_mtx, NULL);

  pthread_attr_init(&attr);
  pthread_create(&monitor_tid, &attr, monitor, NULL);
  for(int i = 0; i < CYCLE; i++){
    params[i] = rand()%5+1;
    pthread_create(&t_tid, &attr, runner, &params[i]);
    pthread_mutex_lock(&print_mtx);
    printf("Main, i=%d, create thread %lu, count=%d.\n", i, t_tid, params[i]);
    pthread_mutex_unlock(&print_mtx);
  }

  pthread_mutex_lock(&print_mtx);
  printf("Type any symbols to exit.\n");
  pthread_mutex_unlock(&print_mtx);
  fgets(str, STR_SIZE, stdin);
  mc = false;
  exit(0);
  return 0;
}

void condition_init(struct condition* x){
  x->x_count = 0;
  sem_init(&x->x_sem, 0, 0);
}

void condition_wait(struct condition* x){
  x->x_count++;
  if(resources_next_count > 0)
    sem_post(&resources_next);
  else
    pthread_mutex_unlock(&resources_mtx);
  sem_wait(&x->x_sem);
  x->x_count--;
}

void condition_signal(struct condition* x){
  if(x->x_count > 0){
    resources_next_count++;
    sem_post(&x->x_sem);
    sem_wait(&resources_next);
    resources_next_count--;
  }
}

bool condition_check_waiting(int count){
  for(int i = 0; i < count; i++)
    if(x[i].x_count > 0) return true;
  
  return false;
}

struct condition* condition_select_waiting(int count){
  int res = count-1;
  for(int i = count-2; i >= 0; i--)
    if(x[i].x_count > x[res].x_count) res = i;

  return &x[res];
}

void decrease_count(int count){
  unsigned long pid = pthread_self();
  pthread_mutex_lock(&resources_mtx);
  if(available_resources < count)
    condition_wait(&x[count-1]);
  available_resources -= count;
  pthread_mutex_lock(&obt_mtx);
  pthread_mutex_lock(&print_mtx);
  printf("Runner %lu, obtain resource count=%d, obt=%d\n", pid, count, ++obt);
  pthread_mutex_unlock(&print_mtx);
  pthread_mutex_unlock(&obt_mtx);
  if(resources_next_count > 0)
    sem_post(&resources_next);
  else
    pthread_mutex_unlock(&resources_mtx);
}

void increase_count(int count){
  unsigned long pid = pthread_self();
  pthread_mutex_lock(&resources_mtx);
  available_resources += count;
  pthread_mutex_lock(&rel_mtx);
  pthread_mutex_lock(&print_mtx);
  printf("Runner %lu, release resource count=%d, rel=%d\n", pid, count, ++rel);
  pthread_mutex_unlock(&print_mtx);
  pthread_mutex_unlock(&rel_mtx);
  while(condition_check_waiting(available_resources))
    condition_signal(condition_select_waiting(available_resources));
  if(resources_next_count > 0)
    sem_post(&resources_next);
  else
    pthread_mutex_unlock(&resources_mtx);
}

void* monitor(void* param){
  while(mc){
    pthread_mutex_lock(&print_mtx);
    printf("Monitor, available_resources=%d\n", available_resources);
    pthread_mutex_unlock(&print_mtx);
    sleep(3);
  }
  pthread_exit(0);
}

void* runner(void* param){
  int count = *((int*)param);
  unsigned long pid = pthread_self();
  if(count <= 0 || count > MAX_RESOURCES){
    pthread_mutex_lock(&print_mtx);
    printf("Runner %lu, invalid count=%d\n", pid, count);
    pthread_mutex_unlock(&print_mtx);
    pthread_exit(0);
  }
 
  decrease_count(count);
  sleep(10);
  increase_count(count);
 
  pthread_exit(0);
}
