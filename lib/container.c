#include "container.h"

void v_allocate_space(struct vector* v){
  if(v->_type <= PRI_TYPE_MIN || v->_type >= PRI_TYPE_MAX) return;
  int t_size = 0;
  void *tmp = NULL;
  
  if(v->_type == PRI_TYPE_INT)
    t_size = sizeof(int);
  else if(v->_type == PRI_TYPE_STRING)
    t_size = sizeof(char*);

  if(!t_size) return;
  tmp = malloc((v->_capacity + SIZE) * t_size);
  memcpy(tmp, v->_v, v->_len * t_size);
  free(v->_v);
  v->_v = tmp;
  v->_capacity += SIZE;
}

void v_push(struct vector *v, const void* val){
  if(v->_type <= PRI_TYPE_MIN || v->_type >= PRI_TYPE_MAX) return;
  if(v->_len == v->_capacity)
    v_allocate_space(v);

  if(v->_type == PRI_TYPE_INT){
    int *p = (int*)v->_v;
    p[v->_len++] = *(const int*)val;
  }
  else if(v->_type == PRI_TYPE_STRING){
    char **p = (char**)v->_v;
    const char *cp = (const char*)val;
    int len = strlen(cp);
    char *tmp = (char*)malloc((len + 1) * sizeof(char));
    strcpy(tmp, cp);
    p[v->_len++] = tmp;
  }
}

void v_clear(struct vector *v){
  if(v->_type <= PRI_TYPE_MIN || v->_type >= PRI_TYPE_MAX) return;
  
  if(v->_type == PRI_TYPE_STRING){
    char **p = (char**)v->_v;
    for(int i = 0; i < v->_len; i++){
      free(p[i]);
    }
  }

  if(v->_v) free(v->_v);
  v->_len = 0;
  v->_capacity = 0;
}

void p_str_init(struct p_string *str){
  if(str->_s) free(str->_s);
  str->_s = (char*)malloc(SIZE * sizeof(char));
  str->_s[0] = '\0';
  str->_capacity = SIZE;
}

void p_str_concat(struct p_string *str, const char *s){
  if(!str->_s)
    p_str_init(str);

  int len = strlen(str->_s), slen = strlen(s);
  int remain = str->_capacity - len - 1;
  if(remain < slen){
    int n = (slen - remain) / SIZE + 1;
    int new_capacity = str->_capacity + n * SIZE;
    char *tmp = (char*)malloc(new_capacity * sizeof(char));
    strcpy(tmp, str->_s);
    p_str_clear(str);
    str->_s = tmp;
    str->_capacity = new_capacity;
  }
  strcpy(&str->_s[len], s);
}

void p_str_clear(struct p_string *str){
  if(str->_s) free(str->_s);
  str->_capacity = 0;
}
