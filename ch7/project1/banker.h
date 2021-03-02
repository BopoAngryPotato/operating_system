#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

int init_banker(int, int, int[]);
int request_resouces(int, int[]);
int release_resouces(int, int[]);
