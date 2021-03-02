#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

extern int NUMBER_OF_CUSTOMERS, NUMBER_OF_RESOURCES;
extern int *available, **maximum, **allocation, **need;

int init_banker(int, int, int[]);
int request_resources(int, int[]);
int release_resources(int, int[]);
