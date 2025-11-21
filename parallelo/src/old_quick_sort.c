void swap(int *data,int first,int second){

	int tmp = data[first];

	data[first]=data[second];

	data[second]=tmp;

}







/**

 * Lomuto partition scheme: single forward pass

 * Pivot = data[alto] (last element)

 * Returns final pivot position

 */

int partition_lomuto(int *data,int basso,int alto){

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





int partition_hoare(int *data, int basso, int alto){

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



/** corpo base di quick sort  per farlo partire con facilit… merge_sort(data,0,size-1)

 * data : il puntatore ai dati 

 * basso l'indice l'estremo inferiore da cui partire

 * alto l'indice l'estremo superiore a cui arrivare , Š escluso

 */

void quick_sort(int *data,int basso,int alto){

  if(basso<alto){

//   int pivot = partition_lomuto(data,basso,alto);

   int pivot = partition_hoare(data,basso,alto);

    quick_sort(data,basso,pivot-1);

    quick_sort(data,pivot+1,alto);

  }



}



void select_median_of_3(int *data, int basso, int alto){

  int mid = (basso+alto)/2;

  if( data[basso] > data[mid] ) swap(data,basso,mid);

  if( data[mid] > data[alto] ) swap(data,mid,alto);

  if( data[basso] > data[mid] ) swap(data,basso,mid);



  swap(data,basso,mid);

}



void median_quick_sort(int *data,int basso,int alto){

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

void quick_sort_omp(int *data,int basso,int alto,int is_median){

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

void quick_sort_omp_start(int *data,int basso,int alto,int is_median){

	#pragma omp parallel

	{

		#pragma omp single  // Solo un thread crea i task iniziali

		quick_sort_omp(data, basso , alto,is_median);

	}



}
