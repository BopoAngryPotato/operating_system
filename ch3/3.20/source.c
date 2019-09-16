#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define SIZE 100

int calculate(int n);

int main(){
  int in;
  char str[SIZE] = { 0 }, c;
  pid_t pid;

  printf("Calculating Collatz conjecture!\n");
  do{
    c = 'y';
    printf("Please type the positive integer to start: ");
    fgets(str, SIZE, stdin);
    in = atoi(str);
    if(in <= 0){
      printf("%d is not a valid value.\n", in);
      continue;
    }
    printf("Recieved value is: %d, start calculating!\n", in);
    pid = fork();
    if(pid < 0){
      fprintf(stderr, "Fork failed.\n");
      return 1;
    }
    else if(pid == 0){
      pid_t cpid = getpid();
      printf("Child process %u forked. Calculating!\n", cpid);
      int n = in;
      printf("Value: %d\n", n);
      while(n != 1){
        n = calculate(n);
        printf("Value: %d\n", n);
      }
      exit(0);
    }
    else{
      wait(NULL);
      printf("Child process %u done calculating!\n", pid);
    }
    printf("Do you want to continue for another calculation (y/n)? ");
    fgets(str, SIZE, stdin);
    c = str[0];
  }while(c == 'y');

  return 0;
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
