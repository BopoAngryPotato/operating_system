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

struct Teaching_assistant {
  enum TA_state x_state;
  sem_t x_sem;
};

void init_ta(struct Teaching_assistant*);
void* teaching_assistant_runner(void*);
void* student_runner(void*);
bool check_ta_and_take_seat(sem_t*);
sem_t* pick_student();

static pthread_mutex_t print_mtx;
static sem_t **seats, *tutored;
static int students_count = 0, seats_taken = 0;
static struct Teaching_assistant ta;

int main(int argc, char** argv){
  char str[STR_SIZE];
  printf("Run Sleeping Teaching Assistant!\n");
  pthread_mutex_init(&print_mtx, NULL);
  seats = (sem_t**)malloc(SEAT_COUNT * sizeof(sem_t*));
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

  pthread_mutex_lock(&print_mtx);
  printf("Type any symbols to exit.\n");
  pthread_mutex_unlock(&print_mtx);
  fgets(str, STR_SIZE, stdin);
  exit(0);

  return 0;
}

void init_ta(struct Teaching_assistant* x){
  x->x_state = SLEEPING;
  sem_init(&x->x_sem, 0, 0);
}
