#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "container.h"

#ifndef P_LIB_PARSER 
#define P_LIB_PARSER
bool is_integer(char*);
void str_split(struct vector*, const char*, const char);
#endif
