#include <stdio.h>
#include "parser.h"

bool is_integer(char* str){
  int len = strlen(str);
  if(len == 0) return false;
  for(int i = 0; i < len; i++){
    if(!isdigit(str[i])) return false;
  }
  return true;
}

void str_split(struct vector *arr, const char *str, const char delim){
  if(arr->_type != PRI_TYPE_STRING) return;
  int len = strlen(str);
  bool looping = true;
  const char *ptr = str;
  char *buf = (char*)malloc((len + 1) + sizeof(char));
  while(looping){
    const char *tmp = strchr(ptr, delim);
    int l;
    if(tmp){
      l = tmp - ptr;
    }
    else{
      l = str + len - ptr;
      looping = false;
    }
    if(l < 0) l = 0;
    strncpy(buf, ptr, l);
    buf[l] = '\0';
    v_push(arr, buf);
    ptr = ptr + l;
    if(tmp) ptr++;
  }
}

int read_file(struct vector *arr, const char *f){
  if(arr->_type != PRI_TYPE_STRING) return 1;
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
  while(fgets(buf, SIZE+1, fp)){
    p_str_concat(&str, buf);
    if(!(strlen(buf) == SIZE && buf[SIZE-1] != '\n' && !feof(fp))){
      v_push(arr, str._s);
      p_str_reset(&str);
    }
  }

EXIT:
  free(buf);
  p_str_clear(&str);
  return ret;
}
