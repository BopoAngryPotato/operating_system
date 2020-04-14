#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#define MAX_RESOURCES 5

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

int available_resourcs = MAX_RESOURCES;
struct sem_list p_s_list;

int main(){
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
  }
  pthread_mutex_unlock(&l->mtx);
}

void destroy_sem_list(struct sem_list* l){
  free(l->head);
  free(l->tail);
}

int decrease_count(int count){
}

int increase_count(int count){
}
