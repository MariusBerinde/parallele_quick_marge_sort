#include "lib.h"
/**
 * funzione usata per scambiare gli elementi in posizione first e second nell'array data.
 * data Ë il puntatore ai dati 
 * first indice del primo elemento
 * second indice del secondo elemento
 */
void swap(long *data,int first,int second){
	int tmp = data[first];
	data[first]=data[second];
	data[second]=tmp;
}



/**
 * Lomuto partition scheme: single forward pass
 * Pivot = data[alto] (last element)
 * Returns final pivot position
 */
int partition_lomuto(long *data,int basso,int alto){
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


int partition_hoare(long *data, int basso, int alto){
  int pivot = data[basso];
  int l = basso + 1;
  int r = alto;
  while(1){
    while(l<r && data[l]<= pivot) l++;
    while(l<r && data[r]>= pivot) r--;
    if( l==r ) break;
    swap(data,l,r);
  }

  if( pivot < data[l]) l--;
  swap(data,basso,l);

  return l;


}

/** corpo base di quick sort  per farlo partire con facilit‡ merge_sort(data,0,size-1)
 * data : il puntatore ai dati 
 * basso l'indice l'estremo inferiore da cui partire
 * alto l'indice l'estremo superiore a cui arrivare , Ë escluso
 */
void quick_sort(long *data,int basso,int alto){
  if(basso<alto){
//   int pivot = partition_lomuto(data,basso,alto);
   int pivot = partition_hoare(data,basso,alto);
    quick_sort(data,basso,pivot-1);
    quick_sort(data,pivot+1,alto);
  }

}

void select_median_of_3(long *data, int basso, int alto){
  int mid = (basso+alto)/2;
  if( data[basso] > data[mid] ) swap(data,basso,mid);
  if( data[mid] > data[alto] ) swap(data,mid,alto);
  if( data[basso] > data[mid] ) swap(data,basso,mid);

  swap(data,basso,mid);
}

void median_quick_sort(long *data,int basso,int alto){
  if(basso < alto){
    select_median_of_3(data,basso,alto);
    int pivot = partition_hoare(data,basso,alto);
    median_quick_sort(data,basso,pivot-1);
    median_quick_sort(data,pivot+1,alto);
  }
}

/* versione parallela di quick sort che utilizza il paradigma omp per funzionare 
 * attenzione : per farlo compilare correttamente serve usare il flag -fopenmpi
 * attenzione : per lanciarlo nel modo corretto bisogna usare la funizone quick_sort_omp_start 
 */
void quick_sort_omp(long *data,int basso,int alto,int is_median){
	if(basso<alto){
//		int pivot = partition_lomuto(data,basso,alto);
		int pivot = partition_hoare(data,basso,alto);
		if(alto - basso >= MIN_ACTIVATION) {
			#pragma omp task shared(data) firstprivate(basso)
			quick_sort_omp(data,basso,pivot-1,is_median);

			#pragma omp task shared(data) firstprivate(alto)
			quick_sort_omp(data,pivot+1,alto,is_median);

			#pragma omp taskwait

		}else{
			if(is_median == MEDIAN_ACTIVATION){
        //printf("[%s] Attivazione median\n",__func__);

							median_quick_sort(data,basso,pivot-1);
							median_quick_sort(data,pivot+1,alto);
			}else{

							quick_sort(data,basso,pivot-1);
							quick_sort(data,pivot+1,alto);
			}
			
		}
	}
}


/* funzione usata per lanciare correttamente quick sort parallel 
 **/
void quick_sort_omp_start(long *data,int basso,int alto,int is_median){
	#pragma omp parallel
	{
		#pragma omp single  // Solo un thread crea i task iniziali
		quick_sort_omp(data, basso , alto,is_median);
	}

}

/*funzione di merge 
 * data indica l'array di partenza
 * low indica l'indice inferiore
 * mid rappresenta la met√† 
 * high rappresenta l'indice superiore
 */

void merge(long* data,int low,int mid,int high){
	int i,j,k;
	int n1 = mid-low+1;
	int n2 = high-mid;
	//int tmpLeft[n1],tmpRight[n2];
	long *tmpLeft = malloc(n1 * sizeof(long));
	long *tmpRight = malloc(n2 * sizeof(long));

	if(tmpLeft == NULL ){
		printf("[%s] Errore con creazione tmpLeft\n",__func__);
		exit(1);
	}

	if( tmpRight == NULL ){
		printf("[%s] Errore con creazione tmpRight\n",__func__);
		exit(1);
	}

	// sono in lettura dei dati quindi posso caricare in contemporanea left e right  possono essere messi in delle sections 


	/*
  for (i=0;i<n1;i++) {
    tmpLeft[i] = data[low+i];
  }
  */


	memcpy(tmpLeft,&data[low],n1*sizeof(long));	
	memcpy(tmpRight,&data[mid+1],n2*sizeof(long));	
	/*
  for(j=0;j<n2;j++){
    tmpRight[j] = data[mid+1+j];
  }
  */


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

	while(i<n1){
		data[k++]= tmpLeft[i++];
		// i++;
		// k++;
	}

	while(j<n2){
		data[k++]= tmpRight[j++];
		//j++;
		//k++;
	}
	free(tmpLeft);
	free(tmpRight);
}


void merge_omp(long *restrict src, long *restrict dst, size_t left, size_t mid, size_t right) {
    size_t i = left, j = mid + 1, k = left;
    
    while (i <= mid && j <= right) {
        dst[k++] = (src[i] <= src[j]) ? src[i++] : src[j++];
    }
    
    if (i <= mid) {
        memcpy(&dst[k], &src[i], (mid - i + 1) * sizeof(long));
    }
    
    if (j <= right) {
        memcpy(&dst[k], &src[j], (right - j + 1) * sizeof(long));
    }
}

/**
  * main corp of merge sort :
  * data Ë l'array dei dati 
  * left Ë l'estremo inferiore su cui effettuare l'ordinamento
  * right Ë l'estremo superiore su cui effettuare l'ordinamento
*/
void merge_sort(long* data,size_t left,size_t right){
  if(left<right){
    int center = (left+right)/2;
    //printf("%s, center = %d\n",__func__,center);
    //indipendente
    merge_sort(data,left,center);
    //indipendente
    merge_sort(data,center+1,right);
    //qui collect di dati 
    merge(data,left,center,right); // Ë possibile parallellelizzarla
    
  }
}
int min(int a,int b){
  return (a<=b)?a:b;
}

void merge_sort_iterative(long *data, size_t size) {
    
    // Bottom-up approach: inizia con blocchi di dimensione 1, poi raddoppia
    for (int block_size = 1; block_size < size; block_size *= 2) {
        
        // Per ogni coppia di blocchi adiacenti di questa dimensione
        for (int left = 0; left < size; left += 2 * block_size) {
            int mid = min(left + block_size - 1, size - 1);
            int right = min(left + 2 * block_size - 1, size - 1);
            
            // Mergia solo se abbiamo effettivamente due blocchi
            if (mid < right) {
                merge(data, left, mid, right );
            }
        }
    }
    
    //free(temp);
}

void merge_sort_omp(long* restrict data,long* restrict tmp_buffer,size_t level,size_t left,size_t right){
	if(left < right) {
		int center = (left + right) / 2;

		if(right - left >= MIN_ACTIVATION && level < 5) {
			#pragma omp task shared(data) firstprivate(left, center)
			merge_sort_omp(data,tmp_buffer,level+1, left, center);

			#pragma omp task shared(data) firstprivate(center, right)  
			merge_sort_omp(data,tmp_buffer,level+1 ,center + 1, right);

			#pragma omp taskwait  
		} else {
			// Per array piccoli, usa versione sequenziale
			merge_sort_iterative(data+left,right-left+1);
			//merge_sort(data, center + 1, right);
		}

		if (level % 2 == 0) {
			merge_omp(data, tmp_buffer, left, center, right);
		} else {
			merge_omp(tmp_buffer, data, left, center, right);
		}


	}

}


/**
 * funzione usata per lanciare correttamene merge sort omp 
 * utilizza una allocazione preventiva del buffer usato per lo scambio dei dati
*/
void merge_sort_omp_start(long* data,size_t left,size_t right){
  size_t n = right-left+1;
  long* tmp_buffer;
  if(posix_memalign((void**)&tmp_buffer, 64, n * sizeof(long))!=0){
    printf("%s : errore con la posix memalig\n",__func__);
    //exit(EXIT_FAILURE);
    tmp_buffer = malloc(n * sizeof(long));
    if( NULL == tmp_buffer){
      printf("%s : errore con la malloc di fallback\n",__func__);
      exit(EXIT_FAILURE);
    }

  }

  #pragma omp parallel 
  {
    #pragma omp single  
    merge_sort_omp(data,tmp_buffer,0, left , right);
  }

  // Se il risultato finale Ë in tmp_buffer, copia in data
  // Dopo livello 0 (pari)  scrive in tmp_buffer
  memcpy(data, tmp_buffer, n * sizeof(long));

  free(tmp_buffer);
}


void gen_random_numbers(long *data,size_t len,long min,long max){
  if( NULL == data ){
    printf("[%s] errore: data null",__func__);
    exit(EXIT_FAILURE);
  }
  const unsigned int SEED = 42; 
  srand(SEED);

    #pragma omp parallel for
    for (size_t i = 0; i < len; i++){
      unsigned int seed = SEED + i; // seed deterministico per ogni indice
      long r = (long)rand_r(&seed);
      data[i] = r % (max - min + 1) + min;
      //data[i]= rand()%(max - min + 1) + min; 
    }
}
