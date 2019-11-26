#include <stdlib.h>
#include <string.h>

#define SIZE 100

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

void allocate_space(struct vector*);
void v_push(struct vector*, const void*);
void v_clear(struct vector*);
