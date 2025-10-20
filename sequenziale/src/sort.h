#include <stdio.h>
#include <stdlib.h>

#include <time.h> // lib per generare numeri random
#include <sys/time.h> // lib per prendere i tempi
#include <string.h>

/**
 * Lomuto partition scheme: single forward pass
 * Pivot = data[alto] (last element)
 * Returns final pivot position
 */
int partition_lomuto(long *data, int basso,int alto);

/**
 * Hoare partition scheme: bidirectional convergence
 * Pivot = data[basso] (first element, or pre-selected)
 * Returns pivot position
 */
int partition_hoare(long *data,int basso,int alto);

void quick_sort(long *data,int basso,int alto);

void select_median_of_3(long *data, int basso, int alto);
void median_quick_sort(long *data,int basso,int alto);

void merge(long* data,size_t low,size_t mid,size_t high);

void merge_sort(long* data,size_t left,size_t right);
