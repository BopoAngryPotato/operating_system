#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>
#include "../../lib/parser.h"

#define RADIUS 1

static int _total, _incircle, _round;

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
  _round = 0;

  #pragma omp parallel for
  for(int i = 0; i < _total; i++){
    unsigned long pid = pthread_self();
    double x, y;
    x = (double)(rand() % SIZE) / SIZE;
    if(rand() % SIZE < SIZE / 2) x = -1 * x;
    y = (double)(rand() % SIZE) / SIZE;
    if(rand() % SIZE < SIZE / 2) y = -1 * y;
    #pragma omp critical
    {
      if(x * x + y * y < RADIUS) _incircle++;
      printf("Runner %lu, round %d, total in circle %d\n", pid, ++_round, _incircle);
    }
  }

  double pi = 4 * (double)_incircle / (double)_total;
  printf("Output: pi = %f, total = %d, incircle = %d\n", pi, _total, _incircle);

  return 0;
}
