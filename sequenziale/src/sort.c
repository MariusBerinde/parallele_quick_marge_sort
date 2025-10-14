#include "sort.h"
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

int random_partition(int *data,int basso,int alto){
  int pos_pivot = rand()%(alto-basso)+basso;
  swap(data,alto,pos_pivot);
  return partition_lomuto(data,basso,alto);

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
/** implementazione di quick sort ricorsivo**/

void quick_sort(int *data,int basso,int alto){
  if(basso<alto){
//    int pivot = partition_lomuto(data,basso,alto);
    int pivot = partition_hoare(data,basso,alto);
//    int pivot = random_partition(data,basso,alto);
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
  //int tmpLeft[n1],tmpRight[n2];
  int *tmpLeft = malloc(n1 * sizeof(int));
  int *tmpRight = malloc(n2 * sizeof(int));

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


  memcpy(tmpLeft,&data[low],n1*sizeof(int));	
  memcpy(tmpRight,&data[mid+1],n2*sizeof(int));	
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

  //la parte di unione delle parti separate non è parallelelizzabile a meno di non trovare delle funzioni di mpi nate per la reduce degli elementi 

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

