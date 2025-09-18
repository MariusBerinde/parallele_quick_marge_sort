#include "lib.h"
/**
 * funzione usata per scambiare gli elementi in posizione first e second nell'array data.
 */
void swap(int *data,int first,int second){
			int tmp = data[first];
			data[first]=data[second];
			data[second]=tmp;
}


/**
 * quick sort ricorsivo
 * basso è l'inizio dell'intervallo
 * alto è la fine dell'intervallo
 * */

int partition(int *data,int basso,int alto){
  int pivot = data[alto];
  int i = basso-1;

  for(int j=basso;j<=alto-1;j++){
    if(data[j]<pivot){
      i++;
      swap(data,i,j);
    }
  }
  swap(data,i+1,alto);
  return (i+1);
}



/** implementazione di quick sort ricorsivo**/

void quick_sort(int *data,int basso,int alto){
  if(basso<alto){
    int pivot = partition(data,basso,alto);
    quick_sort(data,basso,pivot-1);
    quick_sort(data,pivot+1,alto);
  }

}


void quick_sort_omp(int *data,int basso,int alto){
  if(basso<alto){
    int pivot = partition(data,basso,alto);
  if(alto - basso >= MIN_ACTIVATION) {
      #pragma omp task shared(data) firstprivate(basso)
      quick_sort_omp(data,basso,pivot-1);

      #pragma omp task shared(data) firstprivate(alto)
      quick_sort_omp(data,pivot+1,alto);

      #pragma omp taskwait

    }else{

    quick_sort(data,basso,pivot-1);
    quick_sort(data,pivot+1,alto);
    }
  }

}
void quick_sort_omp_start(int *data,int basso,int alto){
#pragma omp parallel
    {
        #pragma omp single  // Solo un thread crea i task iniziali
        quick_sort_omp(data, basso , alto);
    }

}

/*funzione di merge 
 * data indica l'array di partenza
 * low indica l'indice inferiore
 * mid rappresenta la metà 
 * high rappresenta l'indice superiore
 */
void merge(int* data,int low,int mid,int high){
  int i,j,k;
  int n1 = mid-low+1;
  int n2 = high-mid;
  int tmpLeft[n1],tmpRight[n2];

  for (i=0;i<n1;i++) {
    tmpLeft[i] = data[low+i];
  }

  for(j=0;j<n2;j++){
    tmpRight[j] = data[mid+1+j];
  }

  //merge tmpLeft and tmpRight :todo cercare reduce con confronto 
  i=0; j=0; k=low;
  while(i<n1 &&j<n2){
    if(tmpLeft[i] <= tmpRight[j]){
      data[k]=tmpLeft[i];
      i++;
    }else{
      data[k]=tmpRight[j];
      j++;
    }
    k++;

  }

  //la parte di unione delle parti separate non è parallelelizzabile a meno di non trovare delle funzioni di mpi nate per la reduce degli elementi 
  while(i<n1){
    data[k++]= tmpLeft[i++];
    //i++;
    //k++;
  }

  while(j<n2){
    data[k++]= tmpRight[j++];
    //j++;
    //k++;
  }

}

void merge_omp(int* src, int* restrict dst, int left, int mid, int right){

    int i = left, j = mid + 1, k = left;

    while (i <= mid && j <= right) {
        if (src[i] <= src[j]) dst[k++] = src[i++];
        else                  dst[k++] = src[j++];
    }
    while (i <= mid)  dst[k++] = src[i++];
    while (j <= right) dst[k++] = src[j++];
}

/**
  * main corp of merge sort :
  * data è l'array dei dati 
  * left è l'estremo inferiore su cui effettuare l'ordinamento
  * right è l'estremo superiore su cui effettuare l'ordinamento
*/
void merge_sort(int* data,int left,int right){
  if(left<right){
    int center = (left+right)/2;
    //printf("%s, center = %d\n",__func__,center);
    //indipendente
    merge_sort(data,left,center);
    //indipendente
    merge_sort(data,center+1,right);
    //qui collect di dati 
    merge(data,left,center,right); // è possibile parallellelizzarla
    
  }
}


void merge_sort_omp(int* data,int *tmp_buffer,int level,int left,int right){
	if(left < right) {
		int center = (left + right) / 2;

		if(right - left >= MIN_ACTIVATION) {
			#pragma omp task shared(data) firstprivate(left, center)
			merge_sort_omp(data,tmp_buffer,level+1, left, center);

			#pragma omp task shared(data) firstprivate(center, right)  
			merge_sort_omp(data,tmp_buffer,level+1 ,center + 1, right);

			#pragma omp taskwait  
		} else {
			// Per array piccoli, usa versione sequenziale
			merge_sort(data, left, center);
			merge_sort(data, center + 1, right);
		}

		if (level % 2 == 0) {
			merge_omp(data, tmp_buffer, left, center, right);
		} else {
			merge_omp(tmp_buffer, data, left, center, right);
		}


	}

}



void merge_sort_omp_start(int* data,int left,int right){
    int n = right-left+1;
    int* tmp_buffer;
    posix_memalign((void**)&tmp_buffer, 64, n * sizeof(int));

    #pragma omp parallel 
    {
        #pragma omp single  
        merge_sort_omp(data,tmp_buffer,0, left , right);
    }

    // Se il risultato finale � in tmp_buffer, copia in data
    // Dopo livello 0 (pari)  scrive in tmp_buffer
    memcpy(data, tmp_buffer, n * sizeof(int));

    free(tmp_buffer);
}
