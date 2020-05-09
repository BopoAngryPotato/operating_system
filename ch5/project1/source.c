#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <limits.h>
#include "../../lib/parser.h"

#define STR_SIZE 100
#define SEAT_COUNT 3

enum TA_state {
  SLEEPING,
  TUTORING
};

struct Student {
  int index;
  sem_t x_sem;
};

struct Teaching_assistant {
  enum TA_state x_state;
  sem_t x_sem;
  struct Student** seats;
  struct Student* to_tutor;
  int seats_taken;
};

void init_ta(struct Teaching_assistant*);
void init_student(struct Student*, int);
void* teaching_assistant_runner(void*);
void* student_runner(void*);
bool check_ta_and_take_seat(struct Student*);
void pick_student();
int random_time();

static pthread_mutex_t ta_mtx;
static int students_count = 0, _round = 0;;
static struct Teaching_assistant ta;

int main(int argc, char** argv){
  char str[STR_SIZE];
  int* students;
  pthread_t t_tid;
  pthread_attr_t attr;

  printf("Run Sleeping Teaching Assistant!\n");
  pthread_mutex_init(&ta_mtx, NULL);
  init_ta(&ta);

  do{
    printf("Please type number of students looking for help: ");
    fgets(str, STR_SIZE, stdin);
    int len = strlen(str);
    if(len > 0){
      if(str[len-1] == '\n')
        str[len-1] = '\0';
    }
    students_count = atoi(str);
  }while(!is_integer(str) || students_count < 0);
  printf("Type any symbols to exit.\n");

  students = (int*)malloc(students_count * sizeof(int));

  pthread_attr_init(&attr);
  pthread_create(&t_tid, &attr, teaching_assistant_runner, NULL);
  for(int i = 0; i < students_count; i++){
    students[i] = i;
    pthread_create(&t_tid, &attr, student_runner, &students[i]);
  }

  fgets(str, STR_SIZE, stdin);
  exit(0);

  return 0;
}

void init_ta(struct Teaching_assistant* x){
  x->x_state = SLEEPING;
  sem_init(&x->x_sem, 0, 0);
  x->seats = (struct Student**)malloc(SEAT_COUNT*sizeof(struct Student*));
  memset(x->seats, 0, SEAT_COUNT*sizeof(struct Student*));
  x->to_tutor = NULL;
  x->seats_taken = 0;
}

void init_student(struct Student* x, int idx){
  x->index = idx;
  sem_init(&x->x_sem, 0, 0);
}

void* teaching_assistant_runner(void* param){
  while(true){
    sem_wait(&ta.x_sem);
    sleep(random_time());
    pick_student();
  }
  pthread_exit(0);
}

void* student_runner(void* param){
  int idx = *((int*)param);
  struct Student self;
  init_student(&self, idx);
  while(!check_ta_and_take_seat(&self)){
    sleep(random_time());
  }
  sem_wait(&self.x_sem);
  pthread_exit(0);
}

bool check_ta_and_take_seat(struct Student* x){
  bool ret = false;
  pthread_mutex_lock(&ta_mtx);
  if(ta.x_state == SLEEPING){
    ta.x_state = TUTORING;
    ta.to_tutor = x;
    ret = true;
    sem_post(&ta.x_sem);
    printf("Student %d wakes up teaching assistant.\n", x->index);
  }
  else if(ta.seats_taken < SEAT_COUNT){
    ta.seats[ta.seats_taken++] = x;
    ret = true;
    printf("Student %d takes seat, seats taken %d.\n", x->index, ta.seats_taken);
  }
  pthread_mutex_unlock(&ta_mtx);
  return ret;
}

void pick_student(){
  pthread_mutex_lock(&ta_mtx);
  printf("Teaching assistant done tutoring student %d, round %d.\n", ta.to_tutor->index, ++_round);
  if(ta.seats_taken > 0){
    sem_post(&ta.to_tutor->x_sem);
    ta.to_tutor = ta.seats[0];
    ta.seats_taken--;
    for(int i = 0; i < ta.seats_taken; i++){
      ta.seats[i] = ta.seats[i+1];
    }
    ta.seats[ta.seats_taken] = NULL;
    sem_post(&ta.x_sem);
  }
  else{
    ta.to_tutor = NULL;
    ta.x_state = SLEEPING;
    printf("Teaching assistant going to sleep.\n");
  }
  pthread_mutex_unlock(&ta_mtx);
}

int random_time(){
  const int mold = 5;
  return rand() % mold + 1;
}
