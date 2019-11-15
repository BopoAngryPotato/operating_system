#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <limits.h>

#define SIZE 100

struct vector {
  int *_v;
  int _len;
  int _capacity;
};

bool is_integer(char*);
void allocate_space(struct vector*);
void v_push(struct vector*, int);
void* cal_fibonaccis(void*);

static struct vector _fibs = {NULL, 0, 0};
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
    printf("%d", _fibs._v[i]);
    if(i != _fibs._len - 1) printf(", ");
  }
  printf("\n");

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

void allocate_space(struct vector* v){
  int *tmp = (int*)malloc((v->_capacity + SIZE) * sizeof(int));
  memcpy(tmp, v->_v, v->_capacity * sizeof(int));
  free(v->_v);
  v->_v = tmp;
  v->_capacity += SIZE;
}

void v_push(struct vector *v, int n){
  if(v->_len == v->_capacity)
    allocate_space(v);

  v->_v[v->_len++] = n;
}

void* cal_fibonaccis(void* param){
  if(_bound >= 1)
    v_push(&_fibs, 0);

  if(_bound >= 2)
    v_push(&_fibs, 1);

  for(int i = 2; i < _bound; i++){
    int n_1 = _fibs._v[i-1], n_2 = _fibs._v[i-2];
    if(INT_MAX - n_1 < n_2){
      printf("Exceed integer limit! Stop!\n");
      break;
    }
    v_push(&_fibs, n_1 + n_2);
  }
  pthread_exit(0);
}

