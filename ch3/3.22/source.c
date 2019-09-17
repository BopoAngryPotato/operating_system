#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdbool.h>

#define STR_SIZE 100
#define SIZE 10
#define MEM_NAME "OS"

const int space = SIZE * sizeof(int);

void manage_calculation(int in);
int calculate(int n);

int main(){
  int in, n;
  char str[STR_SIZE] = { 0 }, c;

  printf("Calculating Collatz conjecture!\n");
  do{
    c = 'y';
    printf("Please type the positive integer to start: ");
    fgets(str, STR_SIZE, stdin);
    in = atoi(str);
    if(in <= 0){
      printf("%d is not a valid value.\n", in);
      continue;
    }

    bool done = false;
    int shm_fd = shm_open(MEM_NAME, O_CREAT|O_RDWR, 0666);
    ftruncate(shm_fd, space);
    n = in;
    do{
      manage_calculation(n);
      int *ptr = (int*)mmap(0, space, PROT_READ, MAP_SHARED, shm_fd, 0), i;
      for(i = 0; i < SIZE; i++){
        printf("Value: %d\n", ptr[i]);
        if(ptr[i] == 1) break;
      }
      if(i < SIZE){
        done = true;
      }
      else{
        done = false;
        n = calculate(ptr[SIZE-1]);
        printf("Not finished. Continue!\n");
      }
    }while(!done);

    shm_unlink(MEM_NAME);
    printf("Do you want to continue for another calculation (y/n)? ");
    fgets(str, STR_SIZE, stdin);
    c = str[0];
  }while(c == 'y');

  return 0;
}

void manage_calculation(int in){
  printf("Recieved value is: %d, start calculating!\n", in);
  pid_t pid = fork();
  if(pid < 0){
    fprintf(stderr, "Fork failed.\n");
    return;
  }
  else if(pid == 0){
    pid_t cpid = getpid();
    printf("Child process %u forked. Calculating!\n", cpid);
    int n = in, i = 0;
    int shm_fd = shm_open(MEM_NAME, O_RDWR, 0666);
    int *ptr = (int*)mmap(0, space, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if( n <= 0) n = 1;
    while(i < SIZE){
      ptr[i++] = n;
      if(n == 1)
        break;
      else
        n = calculate(n);
    }
    exit(0);
  }
  else{
    wait(NULL);
  }
}

int calculate(int n){
  if(n <= 1) return 1;
  if(n%2){
    return 3*n+1;
  }
  else{
    return n/2;
  }
}
