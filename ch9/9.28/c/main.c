#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "../../../lib/parser.h"

unsigned int PAGE_SIZE = 1 << 12;

int main(int argc, char** argv){
  printf("Run address interpretor!\n");
  if(argc <= 1){
    printf("No argument!\n");
    return 1;
  }
  if(!is_integer(argv[1])){
    printf("Invalid argument!\n");
    return 1;
  }
  unsigned int address = strtoul(argv[1], NULL, 0);
  if(errno == ERANGE){
    printf("Out of range!\n");
    return 1;
  }
  unsigned int page_number = address / PAGE_SIZE;
  unsigned int offset = address % PAGE_SIZE;
  printf("The address %u contains (page size %u):\n", address, PAGE_SIZE);
  printf("Page number = %u\n", page_number);
  printf("Offset = %u\n", offset);
  return 0;
}
