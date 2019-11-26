#include "container.h"

void allocate_space(struct vector* v){
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
    allocate_space(v);

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

  free(v->_v);
  v->_len = 0;
  v->_capacity = 0;
}
