#include <pthread.h>
#include "../../lib/parser.h"

#ifndef BUFFER_H
#define BUFFER_H
typedef int buffer_item;
#define BUFFER_SIZE 5

struct ID {
  int index;
  char title[STR_SIZE];
};

void init_id(struct ID*, int, const char*);
void init_buffer();
int insert_item(buffer_item, struct ID*);
int remove_item(buffer_item *item, struct ID*);
#endif
