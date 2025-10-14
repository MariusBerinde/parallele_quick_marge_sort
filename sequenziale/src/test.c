#include <stdio.h>
#include <stdlib.h>
#include <time.h> // lib per generare numeri random
#include <sys/time.h> // lib per prendere i tempi
#include <string.h>

float tdiff(struct timeval *start,struct timeval *end){
  return (end->tv_sec-start->tv_sec) + 1e-6 * (end->tv_usec-start->tv_usec);
}
void swap(int *data,int first,int second){
			int tmp = data[first];
			data[first]=data[second];
			data[second]=tmp;
}
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


int random_partition(int *data,int basso,int alto){
  int pos_pivot = rand()%(alto-basso)+basso;
  swap(data,alto,pos_pivot);
  return partition(data,basso,alto);

}




/** versione dell'algoritmo dove il pivot è basso */
int partition_alt(int *data,int basso,int alto){
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
    int pivot = partition(data,basso,alto);
//    int pivot = random_partition(data,basso,alto);
//    int pivot = partition_alt(data,basso,alto);
    quick_sort(data,basso,pivot-1);
    quick_sort(data,pivot+1,alto);
  }

}

void mediana(int *data,int basso,int alto){
  int mid = (basso+alto)/2;
  if( data[basso] > data[mid] ) swap(data,basso,alto);
  if( data[mid] > data[alto] ) swap(data,mid,alto);
  if( data[basso] > data[mid] ) swap(data,basso,mid);

  swap(data,basso,mid);
}
void median_quick_sort(int *data,int basso,int alto){
  if(basso < alto){
    mediana(data,basso,alto);
    int pivot = partition_alt(data,basso,alto);
    median_quick_sort(data,basso,pivot-1);
    median_quick_sort(data,pivot+1,alto);
  }
}

void mini_test_quick_sort(){
    int MAX=7;
	int v[]={5,4,4,3,2,1,11};
	printf("[%s]: Vettore prima dell'ordinamento :",__func__);
  
  
	for(size_t i=0;i<MAX;i++){
		printf("[%s]: v[%ld] = %d\t :",__func__,i,v[i]);
	}
  puts("\n");
     
//	quick_sort(v,0,MAX-1);
//  median_quick_sort(v,0,MAX-1);

	printf("[%s]: Vettore dopo oridinamento \n",__func__);
	for(size_t i=0;i<MAX;i++){
		printf("[%s]: v[%ld] = %d\n :",__func__,i,v[i]);
	}

}


void gen_random_numbers(int *array, int len, int min, int max){
    for (int i = 0; i < len; i++)
        array[i] = rand() % (max - min + 1) + min;
}

void big_test_merge_sort(){
  int SIZE = 1<<20; //2^19 elementi 
  srand(time(0));
  int *data = malloc(SIZE * sizeof(int));

	if(data == NULL){
		perror("Errore creazione array data , arresto forzato\n");

		//printf("[%s] Errore creazione array data seq di %d element arresto forzato\n",__func__,SIZE);
		exit(1);
	}
  gen_random_numbers(data,SIZE,0,2*SIZE);
  int non_ordinalto = 1;
  for(int i=0;i<SIZE-1;i++){
    if(data[i]>data[i+1]){
      non_ordinalto=0;
      break;
    }
  }
  if(non_ordinalto==0)
    printf("[%s] vettore di partenza disordinato\n",__func__);
  else
    printf("[%s] vettore di partenza ordinato\n",__func__);

  struct timeval start,end;

  gettimeofday(&start, NULL);

 // quick_sort(data,0,SIZE-1);
  median_quick_sort(data,0,SIZE-1);

  gettimeofday(&end, NULL);
  non_ordinalto=1;

  for(int i=0;i<SIZE-1;i++){
    if(data[i]>data[i+1]){
      non_ordinalto=0;
      break;
    }
  }
  if(non_ordinalto==0)
    printf("[%s] errore con quick sort\n",__func__);
  else{

    printf("[%s] quick sort ok, tempo di esecuzione ordinamento : %0.6f effettuato con %d elementi\n",__func__, tdiff(&start, &end),SIZE);
	}
free(data);
}


int main(){
//	mini_test_quick_sort();
  big_test_merge_sort();
	return 0;
}


