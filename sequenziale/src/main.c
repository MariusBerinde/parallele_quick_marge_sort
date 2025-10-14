#include "sort.h"
#include <locale.h>
//crono e thread sono usate per prendere prestazioni codice
float tdiff(struct timeval *start,struct timeval *end){
  return (end->tv_sec-start->tv_sec) + 1e-6 * (end->tv_usec-start->tv_usec);
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
	median_quick_sort(v,0,MAX-1);
	printf("[%s]: Vettore dopo oridinamento \n",__func__);
	for(size_t i=0;i<MAX;i++){
		printf("[%s]: v[%ld] = %d\n :",__func__,i,v[i]);
	}

}

void mini_test_merge(){
  int v[]={38,27,43,3,9,82,10};
  int MAX=7;

	printf("[%s]: Vettore prima dell'ordinamento :",__func__);
  
  
	for(size_t i=0;i<MAX;i++){
		printf("[%s]: v[%ld] = %d\t :",__func__,i,v[i]);
	}
  puts("\n");
     
	merge_sort(v,0,MAX-1);
	printf("[%s]: Vettore dopo oridinamento \n",__func__);
	for(size_t i=0;i<MAX;i++){
		printf("[%s]: v[%ld] = %d\n :",__func__,i,v[i]);
	}
}




void gen_random_numbers(int *array, int len, int min, int max){
    for (int i = 0; i < len; i++)
        array[i] = rand() % (max - min + 1) + min;
}

/**
	* funzione per testare tempi di sviluppo con 2^19 elementi
*/
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

    printf("[%s] merge sort ok, tempo di esecuzione ordinamento : %0.6f effettuato con %d elementi\n",__func__, tdiff(&start, &end),SIZE);
	}
free(data);
}

void test_big_quick_sort(){

  setlocale(LC_NUMERIC, "");
  int SIZE=1<<30; //2^19 elementi 
  srand(time(0));
  //int data[SIZE];
  int *data;
  data = malloc(SIZE * sizeof(int));
  if(NULL == data){
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
    printf("[%s] quick sort ok, tempo di esecuzione ordinamento : %0.6f effettuato con %'d elementi  \n",__func__, tdiff(&start, &end),SIZE );
	}
}


int main(){
	//test_perror();
//	mini_test_quick_sort();
	// mini_test_merge();

	//big_test_merge_sort();
	test_big_quick_sort();

	return 0;
}


