#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>

#define SIZE 100
#define RADIUS 1

bool is_integer(char*);
void* cal_pi(void*);

static int _total, _incircle;

int main(int argc, char** argv){
  printf("Run Monte Carlo!\n");
  if(argc <= 1){
    printf("No argument.\n");
    return 0;
  }
  else if(!is_integer(argv[1])){
    printf("Invalid argument.\n");
    return 0;
  }
  
  _total = atoi(argv[1]);
  if(_total <= 0){
    printf("Invalid argument.\n");
    return 0;
  }
  _incircle = 0;

  pthread_t tid;
  pthread_attr_t attr;

  pthread_attr_init(&attr);
  pthread_create(&tid, &attr, cal_pi, NULL);
  pthread_join(tid, NULL);

  double pi = 4 * (double)_incircle / (double)_total;
  printf("Output: pi = %f, total = %d, incircle = %d\n", pi, _total, _incircle);

  return 0;
}

bool is_integer(char* str){
  int len = strlen(str);
  if(len == 0) return false;
  for(int i = 0; i < len; i++){
    if(!isdigit(str[i])) return false;
  }
  return true;
}

void* cal_pi(void* param){
  for(int i = 0; i < _total; i++){
    double x, y;
    x = (double)(rand() % SIZE) / SIZE;
    if(rand() % SIZE < SIZE / 2) x = -1 * x;
    y = (double)(rand() % SIZE) / SIZE;
    if(rand() % SIZE < SIZE / 2) y = -1 * y;
    if(x * x + y * y < RADIUS) _incircle++;
  }
  pthread_exit(0);
}
