#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

extern int NUMBER_OF_CUSTOMERS, NUMBER_OF_RESOUCES;

int request_resouces(int, int[]);
int release_resouces(int, int[]);

