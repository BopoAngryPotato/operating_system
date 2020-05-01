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

static pthread_mutex_t barrier_mtx;
static sem_t barrier_next, x_sem;
static int barrier_next_count = 0, x_count = 0;

bool are_params_valid(int, int);
void init(int);
void barrier_point();
void* runner(void*);

static pthread_mutex_t global_mtx;
static int  _total, _batch, _count, _round;

int main(int argc, char** argv){
  char str[STR_SIZE] = { 0 };
  pthread_t t_tid, *tid_pool;
  pthread_attr_t attr;
  pthread_mutex_init(&global_mtx, NULL);

  printf("Run Monte Carlo!\n");
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
    pthread_create(&t_tid, &attr, runner, NULL);
    tid_pool[i] = t_tid;
  }
  for(int i = 0; i < _total; i++)
    pthread_join(tid_pool[i], NULL);

  printf("Finished pthreads = %d, total = %d\n", _round, _total);

  return 0;
}

void init(int N){
}

void barrier_point(){
}

void* runner(void* param){
  barrier_point();
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
