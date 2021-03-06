#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <limits.h>
#include "../../lib/parser.h"
#include "../../lib/container.h"


void* cal_fibonaccis(void*);

static struct vector _fibs = {NULL, 0, 0, PRI_TYPE_INT};
static int _bound;

int main(int argc, char** argv){
  printf("Run Fibonaccis!\n");
  if(argc <= 1){
    printf("No argument.\n");
    return 0;
  }
  else if(!is_integer(argv[1])){
    printf("Invalid argument.\n");
    return 0;
  }
  _bound = atoi(argv[1]);

  pthread_t tid;
  pthread_attr_t attr;

  pthread_attr_init(&attr);
  pthread_create(&tid, &attr, cal_fibonaccis, NULL);
  pthread_join(tid, NULL);

  printf("Output %d numbers in Fibonacci sequence:\n", _fibs._len);
  for(int i = 0; i < _fibs._len; i++){
    int *p = (int*)_fibs._v;
    printf("%d", p[i]);
    if(i != _fibs._len - 1) printf(", ");
  }
  printf("\n");

  return 0;
}

void* cal_fibonaccis(void* param){
  int val;
  if(_bound >= 1){
    val = 0;
    v_push(&_fibs, &val);
  }

  if(_bound >= 2){
    val = 1;
    v_push(&_fibs, &val);
  }

  for(int i = 2; i < _bound; i++){
    int *p = (int*)_fibs._v;
    int n_1 = p[i-1], n_2 = p[i-2];
    if(INT_MAX - n_1 < n_2){
      printf("Exceed integer limit! Stop!\n");
      break;
    }
    val = n_1 + n_2;
    v_push(&_fibs, &val);
  }
  pthread_exit(0);
}
