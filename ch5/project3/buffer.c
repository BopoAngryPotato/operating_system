#include <string.h>
#include <semaphore.h>
#include <stdio.h>
#include "buffer.h"

static buffer_item buffer[BUFFER_SIZE];
static int head, tail;
static pthread_mutex_t mutex;
static sem_t empty, full;

static void print_buffer(){
  if(head == -1) return;
  for(int i = head; i != tail;){
    printf("%d ", buffer[i]);
    i = (i + 1) % BUFFER_SIZE;
  }
  printf("%d", buffer[tail]);
}

void init_id(struct ID *id, int index, const char* title){
  id->index = index;
  strncpy(id->title, title, STR_SIZE);
  id->title[STR_SIZE-1] = '\0';
}

void init_buffer(){
  memset(buffer, 0, sizeof(buffer));
  head = -1;
  tail = -1;
  pthread_mutex_init(&mutex, NULL);
  sem_init(&empty, 0, BUFFER_SIZE);
  sem_init(&full, 0, 0);
}

int insert_item(buffer_item item, struct ID *id){
  int ret = 0;
  sem_wait(&empty);
  pthread_mutex_lock(&mutex);
  if((tail + 1) % BUFFER_SIZE == head){
    ret = -1;
  }
  else if(head == -1){
    head = 0;
    tail = 0;
    buffer[tail] = item;
  }
  else{
    tail = (tail + 1) % BUFFER_SIZE;
    buffer[tail] = item;
  }
  if(ret) printf("Report error condition: %s %d failed insert item %d: ", id->title, id->index, item);
  else printf("%s %d inserted item %d to buffer: ", id->title, id->index, item);
  print_buffer();
  printf("\n");
  pthread_mutex_unlock(&mutex);
  sem_post(&full);
  return ret;
}

int remove_item(buffer_item *item, struct ID *id){
  int ret = 0;
  sem_wait(&full);
  pthread_mutex_lock(&mutex);
  if(head == -1){
    ret = -1;
  }
  else if(head == tail){
    *item = buffer[head];
    head = -1;
    tail = -1;
  }
  else{
    *item = buffer[head];
    head = (head + 1) % BUFFER_SIZE;
  }
  if(ret) printf("Report error condition: %s %d failed remove item %d: ", id->title, id->index, *item);
  else printf("%s %d removed item %d from buffer: ", id->title, id->index, *item);
  print_buffer();
  printf("\n");
  pthread_mutex_unlock(&mutex);
  sem_post(&empty);
  return ret;
}
