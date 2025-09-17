#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h> // lib per generare numeri random
#include <sys/time.h> // lib per prendere i tempi
#define MIN_ACTIVATION 2048

int partition(int *data,int basso,int alto);

void quick_sort(int *data,int basso,int alto);

void merge(int* data,int low,int mid,int high);


void merge_sort(int* data,int left,int right);

void merge_sort_omp(int* data,int left,int right);


void merge_sort_omp_start(int* data,int left,int right);
