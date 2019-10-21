#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#define STR_SIZE 100

#define MIN_PID 300
#define MAX_PID 5000
#define POOL_SIZE 625

#define FAILURE_CODE 1

int allocate_map(void);
int allocate_pid(void);
void release_pid(int pid);

void* monitor(void*);
void* runner(void*);

static char* _p_pool;
bool mc = true;

int main(){
  char str[STR_SIZE] = { 0 }, c;
  pthread_t monitor_tid, s_tid;
  pthread_attr_t attr;

  printf("Run pid manager!\n");
  allocate_map();
  allocate_pid();

  pthread_attr_init(&attr);
  pthread_create(&monitor_tid, &attr, monitor, NULL);

  do{
    c = 'y';
    printf("Please type number of processes to create: ");
    fgets(str, STR_SIZE, stdin);
    int in = atoi(str);
    if(in <= 0){
      printf("%d is not valid.\n", in);
      continue;
    }

    for(int i = 0; i < in; i++)
      pthread_create(&s_tid, &attr, runner, NULL);

    printf("Do you wish to continue (y/n)?");
    fgets(str, STR_SIZE, stdin);
    c = str[0];
  }while(c == 'y');

  mc = false;

  exit(0);
  return 0;
}

int allocate_map(){
  _p_pool = (char*)malloc(sizeof(char)*POOL_SIZE);
  memset(_p_pool, 0, sizeof(char)*POOL_SIZE);
  if(!_p_pool)
    return FAILURE_CODE;
  else
    return 0;
}

int allocate_pid(){
  for(int i = MIN_PID; i < MAX_PID; i++){
    int step = sizeof(char) * 8;
    int index = i / step;
    int pos =  i % step;
    int filter = 1 << pos;
    char block = _p_pool[index];
    if(!(block & filter)){
      _p_pool[index] = block | filter;
      return i;
    }
  }
  return FAILURE_CODE;
}

void release_pid(int pid){
  if(pid < MIN_PID || pid >= MAX_PID) return;
  int step = sizeof(char) * 8;
  int index = pid / step;
  int pos =  pid % step;
  int filter = ~(1 << pos);
  char block = _p_pool[index];
  _p_pool[index] = block & filter;
}

void* monitor(void *param){
  int pid = allocate_pid();
  while(mc){
    int used[MAX_PID] = { 0 }, len = 0;
    for(int i = MIN_PID; i < MAX_PID; i++){
      int step = sizeof(char) * 8;
      int index = i / step;
      int pos =  i % step;
      int filter = 1 << pos;
      char block = _p_pool[index];
      if(block & filter){
        used[len++] = i;
      }
    }
    printf("\nUsed pids: ");
    for(int i = 0; i < len; i++){
      printf("%d, ", used[i]);
    }
    printf("\n");
    sleep(3);
  }

  release_pid(pid);
  pthread_exit(0);
}

void* runner(void* param){
  int pid = allocate_pid();
  if(pid != FAILURE_CODE) sleep(30);
  release_pid(pid);
  pthread_exit(0);
}
