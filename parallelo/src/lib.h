#include <math.h>
//#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h> // lib per prendere i tempi
#include <time.h> // lib per generare numeri random

#define MIN_ACTIVATION 1<<14
#define MEDIAN_ACTIVATION 1 

/**
 * Lomuto partition scheme: single forward pass
 * Pivot = data[alto] (last element)
 * Returns final pivot position
 */
long partition_lomuto(long *data, long basso, long alto);

/**
 * Hoare partition scheme: bidirectional convergence
 * Pivot = data[basso] (first element, or pre-selected)
 * Returns pivot position
 */
long partition_hoare(long *data, long basso, long alto);

void quick_sort(long *data,long basso,long alto);

void select_median_of_3(long *data, long basso, long alto);

void median_quick_sort(long *data,long basso,long alto);

/** funzione di merge 
 * data dati da inviare 
 * lef inizio intervallo 
 * right fine intervallo (size-1)
 */
void merge_sort(long* data,size_t left,size_t right);

void merge_sort_omp(long* data,long *tmp_buffer,size_t level,size_t left,size_t right);

/* versione di quick sort che utilizza omp per funzionare 
 * data è vettore dei dati 
 * basso l'inizio dell'intervallo 
 * alto la fine dell'intervallo 
 * is_median variabile usato per decidere se attivare o meno la versione median of 3 di quick sort 
 */
void quick_sort_omp_start(long *data,int basso,int alto,int is_median);

void quick_sort_omp(long *data,int basso,int alto,int is_median);

void merge_omp(long* restrict src, long* restrict dst, size_t left, size_t mid, size_t right);

/** @brief entry point for the function that perform the merge sort using omp library 
 * @param data : the buffer 
 * @param left : the base of the position where the algoritm 
 * @param right: the top position
 */
void merge_sort_omp_start(long* data,size_t left,size_t right);


/** funzione che genera dati random nell intervallo min max per l'array data usando la libreria openmpi */
void gen_random_numbers(long *data,size_t len,long min,long max);
