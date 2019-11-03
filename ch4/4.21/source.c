#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>

#define SIZE 100

bool arg_parser(char**, int);
bool is_integer(char*, bool);
void* cal_ave(void*);
void* cal_max(void*);
void* cal_min(void*);

static int _ave, _max, _min, _val[SIZE], _len;

int main(int argc, char** argv){
  printf("Run calculator!\n");
  if(argc <= 1){
    printf("Not arguments passed.\n");
    return 0;
  }
  else if(argc > SIZE + 1){
    printf("Only first 100 values considered.\n");
  }
  _len = argc - 1;
  if(arg_parser(argv + 1, _len)){
    printf("Invalid arguments.\n");
    return 0;
  }

  pthread_t ave_tid, max_tid, min_tid;
  pthread_attr_t attr;

  pthread_attr_init(&attr);
  pthread_create(&ave_tid, &attr, cal_ave, NULL);
  pthread_create(&max_tid, &attr, cal_max, NULL);
  pthread_create(&min_tid, &attr, cal_min, NULL);

  pthread_join(ave_tid, NULL);
  pthread_join(max_tid, NULL);
  pthread_join(min_tid, NULL);

  printf("Output:\n");
  printf("Average = %d\n", _ave);
  printf("Max = %d\n", _max);
  printf("Min = %d\n", _min);

  return 0;
}

bool arg_parser(char** arg, int len){
  for(int i = 0; i < len; i++){
    if(!is_integer(arg[i], false)){
      return true;
    }
    _val[i] = atoi(arg[i]);
  }
  return false;
}

bool is_integer(char* str, bool pure){
  int len = strlen(str);
  if(len == 0) return false;
  if(pure){
    for(int i = 0; i < len; i++){
      if(!isdigit(str[i])) return false;
    }
    return true;
  }
  if(isdigit(str[0])){
    if(len == 1) return true;
    return is_integer(str + 1, true);
  }
  else if(str[0] == '+' || str[0] == '-'){
    if(pure) return false;
    return is_integer(str + 1, true);
  }
  else{
    return false;
  }
}

void* cal_ave(void* param){
  int sum = 0;
  for(int i = 0; i < _len; i++){
    sum += _val[i];
  }
  if(_len <= 0) _ave = 0;
  else _ave = sum / _len;
  pthread_exit(0);
}

void* cal_max(void* param){
  int max = 0;
  for(int i = 0; i < _len; i++){
    if(i == 0 || max < _val[i]) max = _val[i];
  }
  _max = max;
  pthread_exit(0);
}

void* cal_min(void* param){
  int min = 0;
  for(int i = 0; i < _len; i++){
    if(i == 0 || min > _val[i]) min = _val[i];
  }
  _min = min;
  pthread_exit(0);
}
