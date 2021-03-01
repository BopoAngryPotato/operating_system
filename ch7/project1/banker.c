#include "banker.h"

int NUMBER_OF_CUSTOMERS, NUMBER_OF_RESOURCES;
pthread_mutex_t mtx;

static int *available, **maximum, **allocation, **need;

int request_resouces(int customer_num, int request[]){
  int ret = 0;
  pthread_mutex_lock(&mtx);
  int *work = (int*)malloc(NUMBER_OF_RESOURCES*sizeof(int));
  memcpy(work, available, NUMBER_OF_RESOURCES*sizeof(int));
  bool *finish=(bool*)malloc(NUMBER_OF_CUSTOMERS*sizeof(bool));
  memset(finish, false, NUMBER_OF_CUSTOMERS*sizeof(bool));
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
      ret = -1;
      break;
    }
  }
  pthread_mutex_unlock(&mtx);
  return ret;
}

int release_resouces(int customer_num, int request[]){
  pthread_mutex_lock(&mtx);
  pthread_mutex_unlock(&mtx);
  return 0;
}
