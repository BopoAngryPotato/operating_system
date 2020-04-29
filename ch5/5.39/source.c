#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <limits.h>
#include "../../lib/parser.h"

#define STR_SIZE 100
#define RADIUS 1

bool are_params_valid(int, int);
void* cal_pi(void*);

static pthread_mutex_t global_mtx;
static int _total, _incircle, _cycles, _count, _round;

int main(int argc, char** argv){
  char str[STR_SIZE] = { 0 };
  pthread_t t_tid, *tid_pool;
  pthread_attr_t attr;
  pthread_mutex_init(&global_mtx, NULL);

  printf("Run Monte Carlo!\n");
  do{
    printf("Please type number of cycles to run: ");
    fgets(str, STR_SIZE, stdin);
    int len = strlen(str);
    str[len-1] = '\0';
    _cycles = atoi(str);
  }while(!is_integer(str) || _cycles <= 0 || _cycles > INT_MAX / 2);
  do{
    printf("Please type number of points for each cycle: ");
    fgets(str, STR_SIZE, stdin);
    int len = strlen(str);
    str[len-1] = '\0';
    _count = atoi(str);
  }while(!is_integer(str) || !are_params_valid(_cycles, _count));

  _total = _cycles * _count;
  _incircle = 0;
  _round = 0;
  tid_pool = (pthread_t*)malloc(_cycles * sizeof(pthread_t));

  pthread_attr_init(&attr);
  for(int i = 0; i < _cycles; i++){
    pthread_create(&t_tid, &attr, cal_pi, NULL);
    tid_pool[i] = t_tid;
  }
  for(int i = 0; i < _cycles; i++)
    pthread_join(tid_pool[i], NULL);

  double pi = 4 * (double)_incircle / (double)_total;
  printf("Output: pi = %f, total = %d, incircle = %d\n", pi, _total, _incircle);

  return 0;
}

void* cal_pi(void* param){
  unsigned long pid = pthread_self();
  int incircle = 0;
  for(int i = 0; i < _count; i++){
    double x, y;
    x = (double)(rand() % SIZE) / SIZE;
    if(rand() % SIZE < SIZE / 2) x = -1 * x;
    y = (double)(rand() % SIZE) / SIZE;
    if(rand() % SIZE < SIZE / 2) y = -1 * y;
    if(x * x + y * y < RADIUS) incircle++;
  }
  pthread_mutex_lock(&global_mtx);
  _incircle += incircle;
  printf("Runner %lu, round %d, in circle count %d, total in circle %d\n", pid, ++_round, incircle, _incircle);
  pthread_mutex_unlock(&global_mtx);
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
