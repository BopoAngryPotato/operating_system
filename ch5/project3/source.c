#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "../../lib/parser.h"
#include "buffer.h"

#define MODULUS 8 

void* producer_runner(void*);
void* consumer_runner(void*);

int main(int argc, char** argv){
  int *producers, *consumers;
  int sleep_time, prod_count, cons_count;
  pthread_t t_tid;
  pthread_attr_t attr;

  printf("Run Producer-Consumer!\n");
  if(argc < 4){
    printf("No arguments.\n");
    return 0;
  }
  if(!is_integer(argv[1]) || !is_integer(argv[2]) || !is_integer(argv[3])){
    printf("Invalid argument: not integer.\n");
    return 0;
  }
  sleep_time = atoi(argv[1]);
  prod_count = atoi(argv[2]);
  cons_count = atoi(argv[3]);
  if(sleep_time <= 0 || prod_count <= 0 || cons_count <= 0){
    printf("Invalid argument: less or equal to 0.\n");
    return 0;
  }

  producers = (int*)malloc(prod_count * sizeof(int));
  consumers = (int*)malloc(cons_count * sizeof(int));
  init_buffer();
  pthread_attr_init(&attr);
  for(int i = 0; i < prod_count; i++){
    producers[i] = i;
    pthread_create(&t_tid, &attr, producer_runner, &producers[i]);
  }
  for(int i = 0; i < cons_count; i++){
    consumers[i] = i;
    pthread_create(&t_tid, &attr, consumer_runner, &consumers[i]);
  }

  sleep(sleep_time);
  exit(0);
  return 0;
}

void* producer_runner(void* param){
  int idx = *((int*)param);
  struct ID *id = (struct ID*)malloc(sizeof(struct ID));
  init_id(id, idx, "Producer");
  buffer_item item;
  while(true){
    sleep(random_number(MODULUS));
    item = random_number(10);
    insert_item(item, id);
  }
  pthread_exit(0);
}

void* consumer_runner(void* param){
  int idx = *((int*)param);
  struct ID *id = (struct ID*)malloc(sizeof(struct ID));
  init_id(id, idx, "Consumer");
  buffer_item item;
  while(true){
    sleep(random_number(MODULUS));
    remove_item(&item, id);
  }
  pthread_exit(0);
}
