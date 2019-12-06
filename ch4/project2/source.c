#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../lib/parser.h"
#include "../../lib/container.h"
int setupByDigits(char**, int);
int setupByFile(char*);
void* mergeSortRunner(void*);
void mergeSort(int, int);
void merge(int, int);
void insertSort(int, int);

typedef struct{
  int left, right;
} parameters;

//int arr[] = {7, 12, 19, 3, 18, 4, 2, 6, 15, 8}, dest[10], len = 10;
int *arr, *dest, len;

int main(int argc, char *argv[]){
  printf("Start merge sort app!\n");
  if(argc <= 2){
    printf("Too few arguments\n");
    return 0;
  }

  if(!strcmp(argv[1], "-d")){
    if(setupByDigits(argv + 2, argc - 2)){
      printf("Failed to setup by digits\n");
      return 0;
    }
  }
  else if(!strcmp(argv[1], "-f")){
    if(setupByFile(argv[2])){
      printf("Failed to setup by file\n");
      return 0;
    }
  }
  else{
    printf("Invalid option\n");
    return 0;
  }

  printf("Income:  ");
  for(int i = 0; i < len; i++){
    printf("%d", arr[i]);
    if(i < len - 1) printf(", ");
    else printf("\n");
  }
  
  mergeSort(0, len-1);

  printf("Outcome: ");
  for(int i = 0; i < len; i++){
    printf("%d", dest[i]);
    if(i < len - 1) printf(", ");
    else printf("\n");
  }

  free(arr);
  free(dest);

  return 0;
}

int setupByDigits(char **digits, int l){
  if(l <= 0) return 1;
  len = l;
  arr = (int*)malloc(len * sizeof(int));
  dest = (int*)malloc(len * sizeof(int));

  for(int i = 0; i < len; i++){
    if(strlen(digits[i]) <= 0 || !is_integer(digits[i])){
      free(arr);
      free(dest);
      return 1;
    }
    arr[i] = atoi(digits[i]);
  }

  return 0;
}

int setupByFile(char *f){
  int ret = 0;
  struct vector content, digits;
  v_init(&content, PRI_TYPE_STRING);
  v_init(&digits, PRI_TYPE_STRING);
  if(read_file(&content, f) || content._len == 0){
    ret = 1;
    goto EXIT;
  }
  char* s = ((char**)content._v)[0];
  int sl = strlen(s);
  if(s[sl-1] == '\n') s[sl-1] = '\0';
  str_split(&digits, s, ',');
  if(digits._len == 0){
    ret = 1;
    goto EXIT;
  }
  len = digits._len;
  arr = (int*)malloc(len * sizeof(int));
  dest = (int*)malloc(len * sizeof(int));
  char** digits_p = (char**)digits._v;
  for(int i = 0; i < digits._len; i++){
    if(strlen(digits_p[i]) <= 0 || !is_integer(digits_p[i])){
      ret = 1;
      free(arr);
      free(dest);
      goto EXIT;
    }
    arr[i] = atoi(digits_p[i]);
  }

EXIT:
  v_clear(&content);
  v_clear(&digits);
  return ret;
}

void* mergeSortRunner(void *param){
  parameters *p = (parameters*)param;
  int left = p->left, right = p->right;
  mergeSort(left, right);
  pthread_exit(0);
}

void mergeSort(int left, int right){
  int l = right - left + 1;
  if(l <= 3)
    insertSort(left, right);
  else{
    int center = (right + left) / 2;
    parameters *l_param = (parameters*)malloc(sizeof(parameters));
    parameters *r_param = (parameters*)malloc(sizeof(parameters));
    l_param->left = left;
    l_param->right = center;
    r_param->left = center + 1;
    r_param->right = right;

    pthread_t l_tid, r_tid;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_create(&l_tid, &attr, mergeSortRunner, l_param);
    pthread_create(&r_tid, &attr, mergeSortRunner, r_param);
    pthread_join(l_tid, NULL);
    pthread_join(r_tid, NULL);

    free(l_param);
    free(r_param);

    merge(left, right);
  }
}

void merge(int left, int right){
  if(left >= right) return;
  int center = (right + left) / 2;
  int leftPos = left, leftEnd = center, rightPos = center + 1, rightEnd = right;
  int pos = left;
  while(leftPos <= leftEnd && rightPos <= rightEnd){
    if(dest[leftPos] <= dest[rightPos]){
      arr[pos++] = dest[leftPos++];
    }
    else{
      arr[pos++] = dest[rightPos++];
    }
  }

  while(leftPos <= leftEnd)
    arr[pos++] = dest[leftPos++];

  while(rightPos <= rightEnd)
    arr[pos++] = dest[rightPos++];

  for(int i = left; i <= right; i++){
    dest[i] = arr[i];
  }
}

void insertSort(int left, int right){
  if(left > right) return;
  dest[left] = arr[left];
  for(int i = left + 1; i <= right; i++){
    int tmp = arr[i], j;
    for(j = i; j > left && tmp < dest[j-1]; j--){
      dest[j] = dest[j-1];
    }
    dest[j] = tmp;
  }
}
