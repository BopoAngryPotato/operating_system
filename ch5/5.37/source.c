#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#define STR_SIZE 100
#define MAX_RESOURCES 5
#define CYCLE 50

struct node {
  sem_t* sem;
  struct node* last;
  struct node* next;
};

struct sem_list {
  pthread_mutex_t mtx;
  struct node* head;
  struct node* tail;
};

void init_sem_list(struct sem_list*);
void push_sem_list(struct sem_list*, sem_t*);
void remove_sem_list(struct sem_list*, sem_t*);
void clear_sem_list(struct sem_list*);
void destroy_sem_list(struct sem_list*);
void broadcast_sem_list(struct sem_list*);

int decrease_count(int);
int increase_count(int);

void* monitor(void*);
void* runner(void*);

int available_resources = MAX_RESOURCES;
struct sem_list p_s_list;
pthread_mutex_t resources_mtx, print_mtx;
bool mc = true;
int params[CYCLE];

int main(){
  char str[STR_SIZE] = { 0 };
  pthread_t monitor_tid, t_tid;
  pthread_attr_t attr;
  printf("Run resource allocator!\n");
  pthread_mutex_init(&resources_mtx, NULL);
  pthread_mutex_init(&print_mtx, NULL);
  init_sem_list(&p_s_list);

  pthread_attr_init(&attr);
  pthread_create(&monitor_tid, &attr, monitor, NULL);
  for(int i = 0; i < CYCLE; i++){
    params[i] = rand()%5+1;
    pthread_create(&t_tid, &attr, runner, &params[i]);
    pthread_mutex_lock(&print_mtx);
    printf("Create thread %lu, count=%d.\n", t_tid, params[i]);
    pthread_mutex_unlock(&print_mtx);
  }

  pthread_mutex_lock(&print_mtx);
  printf("Type any symbols to exit.\n");
  pthread_mutex_unlock(&print_mtx);
  fgets(str, STR_SIZE, stdin);
  mc = false;
  exit(0);
  return 0;
}

void init_sem_list(struct sem_list* l){
  pthread_mutex_init(&l->mtx, NULL);
  l->head = (struct node*)malloc(sizeof(struct node));
  l->tail = (struct node*)malloc(sizeof(struct node));
  l->head->sem = NULL;
  l->head->last = NULL;
  l->head->next = l->tail;
  l->tail->sem = NULL;
  l->tail->last = l->head;
  l->tail->next = NULL;
}

void push_sem_list(struct sem_list* l, sem_t* s){
  pthread_mutex_lock(&l->mtx);
  struct node* pre = l->tail->last;
  struct node* tmp = (struct node*)malloc(sizeof(struct node));
  tmp->sem = s;
  pre->next = tmp;
  tmp->last = pre;
  tmp->next = l->tail;
  l->tail->last = tmp;
  pthread_mutex_unlock(&l->mtx);
}

void remove_sem_list(struct sem_list* l, sem_t* s){
  pthread_mutex_lock(&l->mtx);
  struct node* pre = l->head;
  while(pre->next->next){
    if(pre->next->sem == s){
      struct node* tmp = pre->next;
      pre->next = tmp->next;
      tmp->next->last = pre;
      free(tmp);
      break;
    }
    pre = pre->next;
  }
  pthread_mutex_unlock(&l->mtx);
}

void clear_sem_list(struct sem_list* l){
  pthread_mutex_lock(&l->mtx);
  struct node* cur = l->head->next;
  while(cur->next){
    struct node* pre = cur->last;
    struct node* sub = cur->next;
    pre->next = sub;
    sub->last = pre;
    struct node* td = cur;
    cur = sub;
    free(td);
  }
  pthread_mutex_unlock(&l->mtx);
}

void broadcast_sem_list(struct sem_list* l){
  pthread_mutex_lock(&l->mtx);
  struct node* cur = l->head->next;
  while(cur->next){
    sem_post(cur->sem);
    cur = cur->next;
  }
  pthread_mutex_unlock(&l->mtx);
}

void destroy_sem_list(struct sem_list* l){
  free(l->head);
  free(l->tail);
}

int decrease_count(int count){
  pthread_mutex_lock(&resources_mtx);
  if(available_resources < count){
    pthread_mutex_unlock(&resources_mtx);
    return -1;
  }
  else{
    available_resources -= count;
    pthread_mutex_unlock(&resources_mtx);
    return 0;
  }
}

int increase_count(int count){
  pthread_mutex_lock(&resources_mtx);
  available_resources += count;
  pthread_mutex_unlock(&resources_mtx);
  return 0;
}

void* monitor(void* param){
  while(mc){
    pthread_mutex_lock(&print_mtx);
    printf("Monitor, available_resources=%d\n", available_resources);
    pthread_mutex_unlock(&print_mtx);
    sleep(3);
  }
  pthread_exit(0);
}

void* runner(void* param){
  int count = *((int*)param);
  unsigned long pid = pthread_self();
  if(count <= 0 || count > MAX_RESOURCES){
    pthread_mutex_lock(&print_mtx);
    printf("Runner %lu, invalid count=%d\n", pid, count);
    pthread_mutex_unlock(&print_mtx);
    pthread_exit(0);
  }
  
  sem_t sem;
  sem_init(&sem, 0, 0);
  push_sem_list(&p_s_list, &sem);
 
  while(decrease_count(count)){
    sem_wait(&sem);
  }
  pthread_mutex_lock(&print_mtx);
  printf("Runner %lu, obtain resource count=%d\n", pid, count);
  pthread_mutex_unlock(&print_mtx);
  sleep(10);
  increase_count(count);
 
  pthread_mutex_lock(&print_mtx);
  printf("Runner %lu, release resource count=%d\n", pid, count);
  pthread_mutex_unlock(&print_mtx);
  remove_sem_list(&p_s_list, &sem);
  broadcast_sem_list(&p_s_list);
  pthread_exit(0);
}
