#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../lib/parser.h"
#include "../../lib/container.h"

void* mergeSortRunner(void*);
void mergeSort(int, int);
void merge(int, int);
void insertSort(int, int);

typedef struct{
  int left, right;
} parameters;

int arr[] = {7, 12, 19, 3, 18, 4, 2, 6, 15, 8}, dest[10], len = 10;

int main(int argc, char *argv[]){
  printf("Start merge sort app!\n");
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
  return 0;
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
