#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "../../lib/parser.h"
#include "../../lib/container.h"

#define DIM 9
#define SUB_DIM 3
#define ROW_IDX 9
#define COL_IDX 10

typedef struct {
  int row;
  int column;
} parameters;

int setup(char*);
int setup2(char*);
int parseRow(const char*, int);
int parseRow2(const char*, int);

void* validate_row(void*);
void* validate_column(void*);
void* validate_sub(void*);
bool isValid(const int*);

int _sudoku[DIM][DIM];
bool _results[11] = { false };

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

  if(setup2(argv[1])){
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

  parameters* params[DIM] = { NULL };
  pthread_t r_tid, c_tid, s_tid[DIM];
  pthread_attr_t attr;
  pthread_attr_init(&attr);

  pthread_create(&r_tid, &attr, validate_row, NULL);
  pthread_create(&c_tid, &attr, validate_column, NULL);
  for(int i = 0; i < SUB_DIM; i++){
    for(int j = 0; j < SUB_DIM; j++){
      int idx = i * SUB_DIM + j;
      params[idx] = (parameters*)malloc(sizeof(parameters));
      params[idx]->row = i * SUB_DIM, params[idx]->column = j * SUB_DIM;
      pthread_create(&s_tid[idx], &attr, validate_sub, params[idx]);
    }
  }

  pthread_join(r_tid, NULL);
  pthread_join(c_tid, NULL);
  for(int i = 0; i < DIM; i++){
    pthread_join(s_tid[i], NULL);
    free(params[i]);
  }

  bool valid = true;
  for(int i = 0; i < 11; i++){
    if(!_results[i]){
      valid = false;
      break;
    }
  }

  if(valid){
    printf("Output: Sudoku is valid\n");
  }
  else{
    printf("Output: Sudoku is invalid\n");
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
  while(fgets(buf, SIZE+1, fp)){
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
      if(i >= DIM || parseRow2(str, i++)){
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

int setup2(char* f){
  int ret = 0;
  FILE *fp;
  char *buf = (char*)malloc((SIZE+1) * sizeof(char));
  struct p_string str;
  p_str_init(&str);
  if((fp = fopen(f, "r")) == NULL){
    printf("Cannot open file %s\n", f);
    ret = 1;
    goto EXIT;
  }
  int i = 0;
  while(fgets(buf, SIZE+1, fp)){
    p_str_concat(&str, buf);
    if(!(strlen(buf) == SIZE && buf[SIZE-1] != '\n' && !feof(fp))){
      if(i >= DIM || parseRow2(str._s, i++)){
        printf("Invalid data\n");
        ret = 1;
        goto EXIT;
      }
      p_str_reset(&str);
    }
  }

  if(i != DIM){
    printf("Invalid data\n");
    ret = 1;
    goto EXIT;
  }

EXIT:
  free(buf);
  p_str_clear(&str);
  return ret;
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

int parseRow2(const char *str, int i){
  if(!str) return 1;
  int len = strlen(str);
  char *tmp = NULL;
  if(str[len-1] == '\n'){
    tmp = (char*)malloc((len + 1) + sizeof(char));
    strcpy(tmp, str);
    tmp[len-1] = '\0';
    str = tmp;
  }
  struct vector arr;
  v_init(&arr, PRI_TYPE_STRING);
  str_split(&arr, str, ',');

  int ret = 0;
  if(arr._len != DIM){
    ret = 1;
    goto EXIT;
  }

  char **arr_p = (char**)arr._v;
  for(int j = 0; j < arr._len; j++){
    int l = strlen(arr_p[j]);
    if(l <= 0 || !is_integer(arr_p[j])){
      ret = 1;
      goto EXIT;
    }
    _sudoku[i][j] = atoi(arr_p[j]);
  }

EXIT:
  v_clear(&arr);
  if(tmp) free(tmp);
  return ret;
}

void* validate_row(void* param){
  bool valid = true;
  int arr[DIM];
  for(int i = 0; i < DIM; i++){
    for(int j = 0; j < DIM; j++){
      arr[j] = _sudoku[i][j];
    }
    if(!isValid(arr)){
      valid = false;
      break;
    }
  }
  _results[ROW_IDX] = valid;
  pthread_exit(0);
}

void* validate_column(void* param){
  bool valid = true;
  int arr[DIM];
  for(int j = 0; j < DIM; j++){
    for(int i = 0; i < DIM; i++){
      arr[i] = _sudoku[i][j];
    }
    if(!isValid(arr)){
      valid = false;
      break;
    }
  }
  _results[COL_IDX] = valid;
  pthread_exit(0);
}

void* validate_sub(void* param){
  parameters *pos = (parameters*)param;
  int x = pos->row, y = pos->column;
  if(x < 0 || x + SUB_DIM > DIM) goto EXIT;
  if(y < 0 || y + SUB_DIM > DIM) goto EXIT;
  int idx_x = x / SUB_DIM, idx_y = y / SUB_DIM;
  int idx = idx_x * SUB_DIM + idx_y;
  int arr[DIM];
  for(int i = 0; i < SUB_DIM; i++){
    for(int j = 0; j < SUB_DIM; j++){
      arr[i * SUB_DIM + j] = _sudoku[x+i][y+j];
    }
  }
  _results[idx] = isValid(arr);

EXIT:
  pthread_exit(0);
}

bool isValid(const int *arr){
  bool ret[DIM] = { false };
  for(int i = 0; i < DIM; i++){
    int val = arr[i];
    if(val <= 0 || val > DIM) return false;
    if(ret[val-1]) return false;
    ret[val-1] = true;
  }
  return true;
}
