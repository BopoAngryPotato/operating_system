#include <stdlib.h>
#include <string.h>

#ifndef P_LIB_CONTAINER
#define P_LIB_CONTAINER
//#define SIZE 100
#define SIZE 10

enum primary_type {
  PRI_TYPE_MIN,
  PRI_TYPE_INT,
  PRI_TYPE_STRING,
  PRI_TYPE_MAX,
};

struct vector {
  void* _v;
  int _len;
  int _capacity;
  enum primary_type _type;
};

struct p_string {
  char* _s;
  int _capacity;
};

void v_allocate_space(struct vector*);
void v_push(struct vector*, const void*);
void v_clear(struct vector*);

void p_str_init(struct p_string *str);
void p_str_concat(struct p_string *str, const char*);
void p_str_clear(struct p_string *str);
#endif
