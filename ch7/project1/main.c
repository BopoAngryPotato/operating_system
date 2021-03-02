#include "banker.h"
#include "../../lib/parser.h"

#define number_of_customers 10

void* customer(void*);

int main(int argc, char** argv){
  pthread_t t_tid;
  pthread_attr_t attr;

  printf("Run Banker!\n");
  if(argc <= 1){
    printf("No argument!\n");
    return 1;
  }

  int number_of_resources = argc-1;
  int *resources = (int*)malloc(number_of_resources*sizeof(int));
  for(int i = 0; i < number_of_resources; i++)
    resources[i] = atoi(argv[1+i]);
  init_banker(number_of_customers, number_of_resources, resources);

  pthread_attr_init(&attr);
  int* ids = (int*)malloc(number_of_customers*sizeof(int));
  for(int i = 0; i < number_of_customers; i++){
    ids[i] = i;
    pthread_create(&t_tid, &attr, customer, &ids[i]);
  }
  return 0;
}

void* customer(void* param){
  pthread_exit(0);
}
