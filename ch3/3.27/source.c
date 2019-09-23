#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>

#define BUFFER_SIZE 100
#define SUPER_BUFFER_SIZE 110
#define READ_END 0
#define WRITE_END 1

void str_replace(char*, char, char);

int main(int argn, char** argv){
  char str[BUFFER_SIZE] = { 0 }, s_str[SUPER_BUFFER_SIZE] = { 0 }, inputf[BUFFER_SIZE], copyf[BUFFER_SIZE];
  int fd[2];
  pid_t pid;

  printf("Let's copy files!\n");
  if(argn < 3){
    printf("Plese type the input file: ");
    fgets(str, BUFFER_SIZE, stdin);
    strncpy(inputf, str, BUFFER_SIZE);
    inputf[BUFFER_SIZE-1] = '\0';
    str_replace(inputf, '\n', '\0');

    printf("Plese type the destination file: ");
    fgets(str, BUFFER_SIZE, stdin);
    strncpy(copyf, str, BUFFER_SIZE);
    copyf[BUFFER_SIZE-1] = '\0';
    str_replace(copyf, '\n', '\0');
  }
  else{
    strncpy(inputf, argv[1], BUFFER_SIZE);
    inputf[BUFFER_SIZE-1] = '\0';
  
    strncpy(copyf, argv[2], BUFFER_SIZE);
    copyf[BUFFER_SIZE-1] = '\0';
  }

  if(access(inputf, F_OK)){
    printf("File %s not available\n", inputf);
    return 1;
  }

  if(pipe(fd) == -1){
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
    FILE *fp;
    if((fp = fopen(inputf, "r")) == NULL){
      printf("Cannot open file %s\n", inputf);
      return 1;
    }
    while(fgets(str, BUFFER_SIZE, fp)){
      write(fd[WRITE_END], str, strlen(str));
    }

    fclose(fp);
    close(fd[WRITE_END]);
    printf("Parent: close pipe write end!\n");
    wait(NULL);
  }
  else{
    close(fd[WRITE_END]);
    FILE *fp;
    int n;
    if((fp = fopen(copyf, "w")) == NULL){
      printf("Cannot open file %s\n", copyf);
      return 1;
    }

    while((n = read(fd[READ_END], s_str, BUFFER_SIZE)) > 0){
      s_str[n] = '\0';
      fputs(s_str, fp);
    }
    fclose(fp);
    close(fd[READ_END]);
  }

  return 0;
}

void str_replace(char* f, char old, char new){
  int len = strlen(f);
  for(int i = 0; i < len; i++){
    if(f[i] == old) f[i] = new;
  }
}
