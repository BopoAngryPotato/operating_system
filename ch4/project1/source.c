#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "../../lib/parser.h"
#include "../../lib/container.h"

#define DIM 9
#define SUB_DIM 3

int setup(char*);
int parseRow(const char*, int);

int _sudoku[DIM][DIM];

int main(int argc, char** argv){
  printf("Sudoku Solution Validator!\n");
  if(argc < 2){
    printf("Too few arguments\n");
    return 1;
  }

  if(access(argv[1], F_OK)){
    printf("File %s not accessible\n", argv[1]);
    return 1;
  }

  if(setup(argv[1])){
    printf("Failed to setup Sudoku\n");
    return 1;
  }

  printf("Sudoku data:\n");
  for(int i = 0; i < DIM; i++){
    for(int j = 0; j < DIM; j++){
      printf("%d ", _sudoku[i][j]);
    }
    printf("\n");
  }

  return 0;
}

int setup(char* f){
  FILE *fp;
  if((fp = fopen(f, "r")) == NULL){
    printf("Cannot open file %s\n", f);
    return 1;
  }
  char *str = (char*)malloc((SIZE+1) * sizeof(char)), *buf = (char*)malloc((SIZE+1) * sizeof(char));
  int len = SIZE, pos = 0, i = 0;
  while(fgets(buf, SIZE, fp)){
    strcpy(&str[pos], buf);
    pos += SIZE;
    if(strlen(buf) == SIZE && buf[SIZE-1] != '\n' && !feof(fp)){
      if(pos == len){
        len += SIZE;
        char *tmp = (char*)malloc((len+1) * sizeof(char));
        strcpy(tmp, str);
        free(str);
        str = tmp;
      }
    }
    else{
      pos = 0;
      if(i >= DIM || parseRow(str, i++)){
        printf("Invalid data\n");
        return 1;
      }
    }
  }

  if(i != DIM){
    printf("Invalid data\n");
    return 1;
  }

  free(str);
  free(buf);

  return 0;
}

int parseRow(const char *str, int i){
  if(!str) return 1;
  int j = 0, len = strlen(str);
  if(str[len-1] == '\n') len--;
  const char *ptr = str, *tmp;
  char *buf = (char*)malloc((len+1) + sizeof(char));
  while((tmp = strchr(ptr, ','))){
    int l = tmp - ptr;
    if(l <= 0) return 1;
    strncpy(buf, ptr, l);
    buf[l] = '\0';
    if(!is_integer(buf)) return 1;
    _sudoku[i][j++] = atoi(buf);
    ptr = ptr + l + 1;
  }
  int l = str + len - ptr;
  if(l <= 0) return 1;
  strncpy(buf, ptr, l);
  buf[l] = '\0';
  if(!is_integer(buf)) return 1;
  _sudoku[i][j++] = atoi(buf);
  if(j != DIM) return 1;
 
  free(buf);

  return 0;
}
