#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main(){
  char in;
  pid_t pid, cpid;

  pid = fork();
  if(pid < 0){
    fprintf(stderr, "Fork failed.");
    return 1;
  }
  else if(pid == 0){
    cpid = getpid();
    printf("\nChild process %u executing!\n", cpid);
  }
  else{
    cpid = getpid();
    printf("Parent process %u executing!\n", cpid);
    do{
      int c = 0;
      char tmp;
      printf("Do you wish to exit (y/n)? : ");
      while((tmp = getchar()) != '\n'){
        if(c++ == 0) in = tmp;
      }
    }while(in != 'y');
  }

  return 0;
}
