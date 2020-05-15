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
#define PHILOSOPHER_COUNT 5
#define MODULUS 3

enum Philosopher_state {
  THINKING,
  HUNGRY,
  EATING,
};

void pickup_forks(int);
void return_forks(int);
void test(int);
void init_philosophers();
void* philosopher_runner(void*);

static enum Philosopher_state p_state[PHILOSOPHER_COUNT];
static pthread_cond_t p_self[PHILOSOPHER_COUNT];
static pthread_mutex_t p_mtx, print_mtx; 

int main(int argc, char** argv){
  char str[STR_SIZE];
  int p_identity[PHILOSOPHER_COUNT];
  pthread_t t_tid;
  pthread_attr_t attr;

  printf("Run Dining Philosophers!\n");
  pthread_mutex_init(&print_mtx, NULL);
  init_philosophers();
  
  printf("Type enter to exit.\n");

  pthread_attr_init(&attr);
  for(int i = 0; i < PHILOSOPHER_COUNT; i++){
    p_identity[i] = i;
    pthread_create(&t_tid, &attr, philosopher_runner, &p_identity[i]);
  }
  
  fgets(str, STR_SIZE, stdin);
  exit(0);
  return 0;
}

void pickup_forks(int philosopher_number){
  pthread_mutex_lock(&p_mtx);
  p_state[philosopher_number] = HUNGRY;
  test(philosopher_number);
  while(p_state[philosopher_number] != EATING){
    printf("Philosopher %d === has === to wait to obtain forks, dining philosophers: ", philosopher_number);
    for(int i = 0; i < PHILOSOPHER_COUNT; i++){
      if(p_state[i] == EATING) printf("%d ", i);
    }
    printf("\n");
    pthread_cond_wait(&p_self[philosopher_number], &p_mtx);
  }
  pthread_mutex_unlock(&p_mtx);
}

void return_forks(int philosopher_number){
  pthread_mutex_lock(&p_mtx);
  p_state[philosopher_number] = THINKING;
  printf("Philosopher %d <<< returns <<< his forks, dining philosophers: ", philosopher_number);
  for(int i = 0; i < PHILOSOPHER_COUNT; i++){
    if(p_state[i] == EATING) printf("%d ", i);
  }
  printf("\n");
  test((philosopher_number + 4) % PHILOSOPHER_COUNT);
  test((philosopher_number + 1) % PHILOSOPHER_COUNT);
  pthread_mutex_unlock(&p_mtx);
}

void test(int philosopher_number){
  if((p_state[(philosopher_number + 4) % 5] != EATING) &&
    (p_state[philosopher_number] == HUNGRY) &&
    (p_state[(philosopher_number + 1) % 5] != EATING)){
    p_state[philosopher_number] = EATING;
    printf("Philosopher %d >>> picks up >>> his forks, dining philosophers: ", philosopher_number);
    for(int i = 0; i < PHILOSOPHER_COUNT; i++){
      if(p_state[i] == EATING) printf("%d ", i);
    }
    printf("\n");
    pthread_cond_signal(&p_self[philosopher_number]);
  }
}

void init_philosophers(){
  pthread_mutex_init(&p_mtx, NULL);
  for(int i = 0; i < PHILOSOPHER_COUNT; i++){
    p_state[i] = THINKING;
    pthread_cond_init(&p_self[i], NULL);
  }
}

void* philosopher_runner(void* param){
  int id = *((int*)param);
  while(true){
    sleep(random_number(MODULUS));
    pickup_forks(id);
    sleep(random_number(MODULUS));
    return_forks(id);
  }
  pthread_exit(0);
}
