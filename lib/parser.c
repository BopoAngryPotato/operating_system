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
