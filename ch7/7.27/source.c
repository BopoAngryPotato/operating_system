#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "../../lib/parser.h"

#define MOD 10
#define CROSSING 1

enum farmer {
  NORTH_FARMER,
  SOUTH_FARMER
};

struct node {
  enum farmer f;
  unsigned long id;
  bool en_on_bridge;
  bool ex_on_bridge;
  struct node *pre;
  struct node *next;
};

struct monitor {
  pthread_mutex_t mtx;
  sem_t north_sem;
  int north_num;
  sem_t south_sem;
  int south_num;
  bool on_bridge;
  struct node *waiting_head;
  struct node *waiting_tail;
  struct node *enter;
  int en_pos;
  struct node *exit;
  int ex_pos;
  int len;
};

void monitor_init(struct monitor*, int);
void monitor_enter_bridge(struct monitor*, enum farmer, unsigned long);
void monitor_exit_bridge(struct monitor*, enum farmer, unsigned long);
void* farmer(void*);

static struct monitor mn;

int main(int argc, char** argv){
  pthread_t t_tid, *tid_pool;
  pthread_attr_t attr;

  printf("Run bridge!\n");
  if(argc <= 1){
    printf("No argument!\n");
    return 1;
  }
  if(!is_integer(argv[1])){
    printf("Argument is not integer!\n");
    return 1;
  }
  int total = atoi(argv[1]);
  if(total <= 0){
    printf("Argument is negative or zero!\n");
    return 1;
  }
  tid_pool = (pthread_t*)malloc(total * sizeof(pthread_t));
  monitor_init(&mn, total);
  
  pthread_attr_init(&attr);
  for(int i = 0; i < total; i++){
    pthread_create(&t_tid, &attr, farmer, NULL);
    tid_pool[i] = t_tid;
  }
  for(int i = 0; i < total; i++)
    pthread_join(tid_pool[i], NULL);

  printf("Enter: ");
  for(int i = 0; i < total; i++)
    printf("(%lu, %d, %d) ", mn.enter[i].id, mn.enter[i].f, mn.enter[i].en_on_bridge);
  printf("\n");
  printf("Exit:  ");
  for(int i = 0; i < total; i++)
    printf("(%lu, %d, %d, %d) ", mn.exit[i].id, mn.exit[i].f, mn.exit[i].en_on_bridge, mn.exit[i].ex_on_bridge);
  printf("\n");

  return 0;
}

void monitor_init(struct monitor* m, int total){
  pthread_mutex_init(&m->mtx, NULL);
  sem_init(&m->north_sem, 0, 0);
  m->north_num = 0;
  sem_init(&m->south_sem, 0, 0);
  m->south_num = 0;
  m->on_bridge = false;
  m->waiting_head = (struct node*)malloc(sizeof(struct node));
  m->waiting_tail = (struct node*)malloc(sizeof(struct node));
  m->waiting_head->pre = NULL;
  m->waiting_head->next = m->waiting_tail;
  m->waiting_tail->pre = m->waiting_head;
  m->waiting_tail->next = NULL;
  m->enter = (struct node*)malloc(total * sizeof(struct node));
  m->exit = (struct node*)malloc(total * sizeof(struct node));
  m->len = total;
  m->en_pos = 0;
  m->ex_pos = 0;
}

static void monitor_print_waiting(struct monitor* m){
  printf("waiting: ");
  struct node *itr = m->waiting_head->next;
  while(itr && itr->next){
    printf("(%lu, %d) ", itr->id, itr->f);
    itr = itr->next;
  }
}

void monitor_enter_bridge(struct monitor* m, enum farmer f, unsigned long id){
  if(f < NORTH_FARMER || f > SOUTH_FARMER) pthread_exit(0);
  pthread_mutex_lock(&m->mtx);
  if(m->en_pos < 0 || m->en_pos >= m->len){
    pthread_mutex_unlock(&m->mtx);
    pthread_exit(0);
  }
  int i = m->en_pos;
  m->enter[i].f = f;
  m->enter[i].id = id;
  m->en_pos++;
  printf("(%lu, %d) ENter bridge\n", id, f);
  if(m->on_bridge){
    m->enter[i].en_on_bridge = true;
    struct node* tmp = (struct node*)malloc(sizeof(struct node));
    tmp->f = f;
    tmp->id = id;
    struct node* pre = m->waiting_tail->pre;
    m->waiting_tail->pre = tmp;
    tmp->next = m->waiting_tail;
    tmp->pre = pre;
    pre->next = tmp;
    printf("(%lu, %d) Wait, ", id, f);
    monitor_print_waiting(m);
    printf("\n");
    if(f == NORTH_FARMER){
      m->north_num++;
      pthread_mutex_unlock(&m->mtx);
      sem_wait(&m->north_sem);
    }
    else{
      m->south_num++;
      pthread_mutex_unlock(&m->mtx);
      sem_wait(&m->south_sem);
    }
    struct node *itr = m->waiting_head->next;
    while(itr && itr->next){
      if(itr->id == id) break;
      itr = itr->next;
    }
    if(itr->next){
      itr->pre->next = itr->next;
      itr->next->pre = itr->pre;
      free(itr);
    }
    printf("(%lu, %d) Resume, ", id, f);
    monitor_print_waiting(m);
    printf("\n");
    pthread_mutex_unlock(&m->mtx);
  }
  else{
    m->enter[i].en_on_bridge = false;
    m->on_bridge = true;
    pthread_mutex_unlock(&m->mtx);
  }
}

void monitor_exit_bridge(struct monitor* m, enum farmer f, unsigned long id){
  if(f < NORTH_FARMER || f > SOUTH_FARMER) pthread_exit(0);
  pthread_mutex_lock(&m->mtx);
  if(m->ex_pos < 0 || m->ex_pos >= m->len){
    pthread_mutex_unlock(&m->mtx);
    pthread_exit(0);
  }
  int i = m->ex_pos;
  m->exit[i].f = f;
  m->exit[i].id = id;
  for(int j = 0; j < m->en_pos; j++)
    if(m->enter[j].id == id){
      m->exit[i].en_on_bridge = m->enter[j].en_on_bridge;
      break;
    }
  m->ex_pos++;
  int *self, *other;
  sem_t *self_sem, *other_sem;
  printf("(%lu, %d) EXit bridge\n", id, f);
  if(f == NORTH_FARMER){
    self = &m->north_num;
    self_sem = &m->north_sem;
    other = &m->south_num;
    other_sem = &m->south_sem;
  }
  else{
    self = &m->south_num;
    self_sem = &m->south_sem;
    other = &m->north_num;
    other_sem = &m->north_sem;
  }
  if(*other > 0){
    m->exit[i].ex_on_bridge = true;
    (*other)--;
    sem_post(other_sem);
  }
  else if(*self > 0){
    m->exit[i].ex_on_bridge = true;
    (*self)--;
    sem_post(self_sem);
  }
  else{
    m->exit[i].ex_on_bridge = false;
    m->on_bridge = false;
    pthread_mutex_unlock(&m->mtx);
  }
}

void* farmer(void* param){
  unsigned long pid = pthread_self();
  enum farmer f;
  if(random_number(MOD) < MOD/2) f = NORTH_FARMER;
  else f = SOUTH_FARMER;
  sleep(random_number(MOD));
  monitor_enter_bridge(&mn, f, pid);
  sleep(CROSSING);
  monitor_exit_bridge(&mn, f, pid);
  pthread_exit(0);
}
