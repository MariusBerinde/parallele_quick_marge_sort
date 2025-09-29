#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h> // lib per prendere i tempi
#include <time.h> // lib per generare numeri random
#define MIN_ACTIVATION 1<<14

int partition(int *data,int basso,int alto);

void quick_sort(int *data,int basso,int alto);

void merge(int* data,int low,int mid,int high);

void merge_sort(int* data,int left,int right);

void merge_sort_omp(int* data,int *tmp_buffer,int level,int left,int right);

void quick_sort_omp_start(int *data,int basso,int alto);

void quick_sort_omp(int *data,int basso,int alto);

void merge_omp(int* restrict src, int* restrict dst, int left, int mid, int right);

/**
 * @brief entry point for the function that perform the merge sort using omp library 
 * @param data : the buffer 
 * @param left : the base of the position where the algoritm 
 * @param right: the top position
 */
void merge_sort_omp_start(int* data,int left,int right);


void merge_sort_alt(int *data,int left,int right);


void merge_sort_iterative(int *data, int size);


 
