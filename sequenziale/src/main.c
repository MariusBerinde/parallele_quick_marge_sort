#include "sort.h"
#include <locale.h>
#include <omp.h>
//crono e thread sono usate per prendere prestazioni codice
float tdiff(struct timeval *start,struct timeval *end){
  return (end->tv_sec-start->tv_sec) + 1e-6 * (end->tv_usec-start->tv_usec);
}


void mini_test_quick_sort(){
  size_t MAX=7;
	long v[]={5,4,4,3,2,1,11};
	printf("[%s]: Vettore prima dell'ordinamento :",__func__);
  
  
	for(size_t i=0;i<MAX;i++){
		printf("[%s]: v[%ld] = %ld\t :",__func__,i,v[i]);
	}
  puts("\n");
     
//	quick_sort(v,0,MAX-1);
	median_quick_sort(v,0,MAX-1);
	printf("[%s]: Vettore dopo oridinamento \n",__func__);
	for(size_t i=0;i<MAX;i++){
		printf("[%s]: v[%ld] = %ld\n :",__func__,i,v[i]);
	}

}

void mini_test_merge(){
  long v[]={38,27,43,3,9,82,10};
  size_t MAX=7;

	printf("[%s]: Vettore prima dell'ordinamento :",__func__);
  
  
	for(size_t i=0;i<MAX;i++){
		printf("[%s]: v[%ld] = %ld\t :",__func__,i,v[i]);
	}
  puts("\n");
     
	merge_sort(v,0,MAX-1);
	printf("[%s]: Vettore dopo oridinamento \n",__func__);
	for(size_t i=0;i<MAX;i++){
		printf("[%s]: v[%ld] = %ld\n :",__func__,i,v[i]);
	}
}



void gen_random_numbers(long *data,size_t len,long min,long max){
  if( NULL == data ){
    printf("[%s] errore: data null",__func__);
    exit(EXIT_FAILURE);
  }
  const unsigned int SEED = 42; 
  srand(SEED);

  for (size_t i = 0; i < len; i++)
    data[i] = (long)rand() % (max - min + 1) + min;
}



/**
	* funzione per testare tempi di sviluppo con 2^19 elementi
*/
void big_test_merge_sort(){
  size_t SIZE = 1<<20; //2^19 elementi 
  srand(time(0));
  long *data = malloc(SIZE * sizeof(long));

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

	merge_sort(data,0,SIZE-1);

  gettimeofday(&end, NULL);
  non_ordinalto=1;

  for(int i=0;i<SIZE-1;i++){
    if(data[i]>data[i+1]){
      non_ordinalto=0;
      break;
    }
  }
  if(non_ordinalto==0)
    printf("[%s] errore con merge sort\n",__func__);
  else{

    printf("[%s] merge sort ok, tempo di esecuzione ordinamento : %0.6f effettuato con %ld elementi\n",__func__, tdiff(&start, &end),SIZE);
	}
free(data);
}

void test_big_quick_sort(){

  size_t SIZE=1<<30; //2^19 elementi 
  srand(time(0));
  long *data;
  data = malloc(SIZE * sizeof(long));
  if( NULL == data ){
    printf("[%s] problemi con la creazione di data\n",__func__);
    exit(EXIT_FAILURE);
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
  //quick_sort(data,0,SIZE-1);
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
    printf("[%s] quick sort ok, tempo di esecuzione ordinamento : %0.6f effettuato con %ld elementi  \n",__func__, tdiff(&start, &end),SIZE );
	}
}


void printArray(long *data,size_t LEN){
  printf("[%s] stampa array:{\t",__func__);
  size_t i;
  for( i=0;i<LEN-1;i++)
    printf("%ld,",data[i]);
  printf("%ld }\n",data[i]);
}

int main(){
//	test_perror();
//  mini_test_quick_sort();
//	mini_test_merge();
//	big_test_merge_sort();
	test_big_quick_sort();

	return 0;
}


