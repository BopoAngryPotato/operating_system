#include "banker.h"
#include "../../lib/parser.h"

#define SLEEP_MOD 3
#define number_of_customers 5 

#define STR_SIZE 100
#define END "exit\n"

void* customer(void*);

int main(int argc, char** argv){
  char str[STR_SIZE] = { 0 };
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
  do{
    fgets(str, STR_SIZE, stdin);
  }while(strcmp(str, END));
  printf("Exiting!\n");
  return 0;
}

void* customer(void* param){
  int id = *(int*)param;
  while(true){
    sleep(random_number(SLEEP_MOD));
    int *request = (int*)malloc(NUMBER_OF_RESOURCES*sizeof(int));
   
    for(int i = 0; i < NUMBER_OF_RESOURCES; i++)
      request[i] = random_number(need[id][i]+1)-1;
    request_resources(id, request);
   
    sleep(random_number(SLEEP_MOD));
    
    for(int i = 0; i < NUMBER_OF_RESOURCES; i++)
      request[i] = random_number(allocation[id][i]+1)-1;
    release_resources(id, request);
    free(request);
  }
  pthread_exit(0);
}
