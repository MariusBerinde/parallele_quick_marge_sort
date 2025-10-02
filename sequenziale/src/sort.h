#include <stdio.h>
#include <stdlib.h>

#include <time.h> // lib per generare numeri random
#include <sys/time.h> // lib per prendere i tempi
#include <string.h>

int partition(int *data,int basso,int alto);

void quick_sort(int *data,int basso,int alto);

void merge(int* data,int low,int mid,int high);


void merge_sort(int* data,int left,int right);
