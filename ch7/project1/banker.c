#include "banker.h"
#include "../../lib/parser.h"

int NUMBER_OF_CUSTOMERS, NUMBER_OF_RESOURCES;
int *available, **maximum, **allocation, **need;

static pthread_mutex_t mtx;

int init_banker(int number_of_customers, int number_of_resources, int resources[]){
  if(number_of_customers <= 0 || number_of_resources <= 0) return -1;
  for(int i = 0; i < number_of_resources; i++)
    if(resources[i] <= 0) return -1;

  pthread_mutex_init(&mtx, NULL);
  NUMBER_OF_CUSTOMERS = number_of_customers;
  NUMBER_OF_RESOURCES = number_of_resources;
  available = (int*)malloc(NUMBER_OF_RESOURCES*sizeof(int));
  maximum = (int**)malloc(NUMBER_OF_CUSTOMERS*sizeof(int*));
  allocation = (int**)malloc(NUMBER_OF_CUSTOMERS*sizeof(int*));
  need = (int**)malloc(NUMBER_OF_CUSTOMERS*sizeof(int*));
  for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++){
    maximum[i] = (int*)malloc(NUMBER_OF_RESOURCES*sizeof(int));
    allocation[i] = (int*)malloc(NUMBER_OF_RESOURCES*sizeof(int));
    need[i] = (int*)malloc(NUMBER_OF_RESOURCES*sizeof(int));
  }
  for(int i = 0; i < NUMBER_OF_RESOURCES; i++){
    available[i] = resources[i];
    for(int j = 0; j < NUMBER_OF_CUSTOMERS; j++){
      maximum[j][i] = random_number(resources[i]);
      allocation[j][i] = 0;
      need[j][i] = maximum[j][i];
    }
  }
  return 0;
}

int request_resources(int customer_num, int request[]){
  pthread_mutex_lock(&mtx);
  int ret = -1;
  if(customer_num < 0 || customer_num >= NUMBER_OF_CUSTOMERS) goto EXIT;
  for(int i = 0; i < NUMBER_OF_RESOURCES; i++)
    if(request[i] < 0 || request[i] > need[customer_num][i]) goto EXIT;

  bool allowed = true;
  for(int i = 0; i < NUMBER_OF_RESOURCES; i++){
    if(request[i] > available[i]){
      allowed = false;
      goto DECIDE;
    }
  }
  int *work = (int*)malloc(NUMBER_OF_RESOURCES*sizeof(int));
  memcpy(work, available, NUMBER_OF_RESOURCES*sizeof(int));
  bool *finish=(bool*)malloc(NUMBER_OF_CUSTOMERS*sizeof(bool));
  memset(finish, false, NUMBER_OF_CUSTOMERS*sizeof(bool));
  for(int i = 0; i < NUMBER_OF_RESOURCES; i++){
    work[i] -= request[i];
    allocation[customer_num][i] += request[i];
    need[customer_num][i] -= request[i];
  }
  for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++){
    for(int j = 0; j < NUMBER_OF_CUSTOMERS; j++){
      if(!finish[j]){
        bool tmp = true;
        for(int k = 0; k < NUMBER_OF_RESOURCES; k++){
          if(need[j][k] > work[k]){
            tmp = false;
            break;
          }
        }
        if(tmp){
          finish[j] = true;
          for(int k = 0; k < NUMBER_OF_RESOURCES; k++)
            work[k] += allocation[j][k];
        }
      }
    }
  }
  for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++){
    if(!finish[i]){
      allowed = false;
      break;
    }
  }
  for(int i = 0; i < NUMBER_OF_RESOURCES; i++){
    if(allowed){
      available[i] -= request[i];
    }
    else{
      allocation[customer_num][i] -= request[i];
      need[customer_num][i] += request[i];
    }
  }
  free(work);
  free(finish);
DECIDE:
  if(allowed){
    ret = 0;
    printf("Request by %d granted: ", customer_num);
  }
  else{
    printf("Request by %d denied : ", customer_num);
  }
  for(int i = 0; i < NUMBER_OF_RESOURCES; i++)
    printf("%d ", request[i]);
  printf("\n");
EXIT:
  pthread_mutex_unlock(&mtx);
  return ret;
}

int release_resources(int customer_num, int request[]){
  pthread_mutex_lock(&mtx);
  int ret = -1;
  if(customer_num < 0 || customer_num >= NUMBER_OF_CUSTOMERS) goto EXIT;
  for(int i = 0; i < NUMBER_OF_RESOURCES; i++)
    if(request[i] < 0 || request[i] > allocation[customer_num][i]) goto EXIT;
  
  for(int i = 0; i < NUMBER_OF_RESOURCES; i++){
    available[i] += request[i];
    allocation[customer_num][i] -= request[i];
    need[customer_num][i] += request[i];
  }
  ret = 0;
  printf("Release by %d        : ", customer_num);
  for(int i = 0; i < NUMBER_OF_RESOURCES; i++)
    printf("%d ", request[i]);
  printf("\n");
EXIT:
  pthread_mutex_unlock(&mtx);
  return ret;
}
