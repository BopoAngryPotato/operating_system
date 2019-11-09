#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>

#define SIZE 100

bool is_integer(char*);
void* cal_prime(void*);
void allocate_space();

static int *_primes, _len, _capacity, _bound;

int main(int argc, char** argv){
  printf("Run prime numbers!\n");
  if(argc <= 1){
    printf("No argument.\n");
    return 0;
  }
  else if(!is_integer(argv[1])){
    printf("Invalid argument.\n");
    return 0;
  }
  _bound = atoi(argv[1]);
  _len = 0;
  _capacity = 0;

  pthread_t tid;
  pthread_attr_t attr;

  pthread_attr_init(&attr);
  pthread_create(&tid, &attr, cal_prime, NULL);
  pthread_join(tid, NULL);

  printf("Output Prime Numbers less than or equal to %d:\n", _bound);
  for(int i = 0; i < _len; i++){
    printf("%d", _primes[i]);
    if(i != _len - 1) printf(", ");
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

void* cal_prime(void* param){
  for(int i = 1; i <= _bound; i++){
    bool isPrime = true;
    for(int j = 1; j < _len; j++){
      if(!(i % _primes[j])){
        isPrime = false;
        break;
      }
    }
    if(isPrime){
      if(_len == _capacity)
        allocate_space();

      _primes[_len++] = i;
    }
  }
  pthread_exit(0);
}

void allocate_space(){
  int *tmp = (int*)malloc((_capacity + SIZE) * sizeof(int));
  memcpy(tmp, _primes, _capacity * sizeof(int));
  free(_primes);
  _primes = tmp;
  _capacity += SIZE;
}
