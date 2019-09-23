#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <stdlib.h>

#define BUFFER_SIZE 100
#define READ_END 0
#define WRITE_END 1

void revert(const char*, char*, int);

int main(){
  char read_msg[BUFFER_SIZE], r_read_msg[BUFFER_SIZE];
  char str[BUFFER_SIZE] = { 0 }, c;
  int fd[2], r_fd[2];
  pid_t pid;

  printf("Let's revert charactors!\n");

  if(pipe(fd) == -1 || pipe(r_fd) == -1){
    fprintf(stderr, "Pipe failed.");
    return 1;
  }

  pid = fork();

  if(pid < 0){
    fprintf(stderr, "Fork failed.");
    return 1;
  }

  if(pid > 0){
    close(fd[READ_END]);
    close(r_fd[WRITE_END]);
    do{
      printf("Please type a sentence: ");
      fgets(str, BUFFER_SIZE, stdin);

      char write_msg[BUFFER_SIZE];
      strncpy(write_msg, str, BUFFER_SIZE);
      write_msg[BUFFER_SIZE-1] = '\0';
      write(fd[WRITE_END], write_msg, strlen(write_msg)+1);
      
      read(r_fd[READ_END], r_read_msg, BUFFER_SIZE);
      r_read_msg[BUFFER_SIZE-1] = '\0';
      printf("Reverted sentence: %s\n", r_read_msg);
      printf("Do you want to continue (y/n)? ");
      fgets(str, BUFFER_SIZE, stdin);
      c = str[0];
    }while(c == 'y');

    kill(pid, SIGKILL);
  }
  else{
    close(fd[WRITE_END]);
    close(r_fd[READ_END]);
    while(read(fd[READ_END], read_msg, BUFFER_SIZE) > 0){
      read_msg[BUFFER_SIZE-1] = '\0';
      char r_write_msg[BUFFER_SIZE];
      revert(read_msg, r_write_msg, BUFFER_SIZE);
      write(r_fd[WRITE_END], r_write_msg, strlen(r_write_msg)+1);
    }
    fprintf(stderr, "Child process read pipe error.");
    exit(1);
  }

  return 0;
}

void revert(const char* in, char* out, int size){
  int i;
  for(i = 0; in[i] != '\0' && i < size - 1; i++){
    if(isalpha(in[i])){
      if(islower(in[i])){
        out[i] = 'A' + in[i] - 'a';
      }
      else if(isupper(in[i])){
        out[i] = 'a' + in[i] - 'A';
      }
    }
    else{
      out[i] = in[i];
    }
  }
  out[i] = '\0';
}
