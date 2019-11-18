#include <stdlib.h>
#include <string.h>

#define SIZE 100

struct vector {
  int *_v;
  int _len;
  int _capacity;
};

void allocate_space(struct vector*);
void v_push(struct vector*, int);
