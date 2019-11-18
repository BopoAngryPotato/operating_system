#include "container.h"

void allocate_space(struct vector* v){
  int *tmp = (int*)malloc((v->_capacity + SIZE) * sizeof(int));
  memcpy(tmp, v->_v, v->_capacity * sizeof(int));
  free(v->_v);
  v->_v = tmp;
  v->_capacity += SIZE;
}

void v_push(struct vector *v, int n){
  if(v->_len == v->_capacity)
    allocate_space(v);

  v->_v[v->_len++] = n;
}
