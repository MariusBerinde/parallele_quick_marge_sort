#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>
#include <time.h> // lib per generare numeri random
#include <sys/time.h> // lib per prendere i tempi
#define MIN_ACTIVATION 100000

int partition(int *data,int basso,int alto);

void quick_sort(int *data,int basso,int alto);

void merge(int* data,int low,int mid,int high);

void merge_sort(int* data,int left,int right);

void merge_sort_omp(int* data,int *tmp_buffer,int level,int left,int right);

void quick_sort_omp_start(int *data,int basso,int alto);

void quick_sort_omp(int *data,int basso,int alto);

void merge_omp(int* restrict src, int* restrict dst, int left, int mid, int right);

/** entry point for the function that perform the merge sort using omp library 
 * data : the buffer 
 * left : the base of the position where the algoritm 
 * right: the top position
 */
void merge_sort_omp_start(int* data,int left,int right);
