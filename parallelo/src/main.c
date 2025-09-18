#include "lib.h"

//crono e thread sono usate per prendere prestazioni codice
float tdiff(struct timeval *start,struct timeval *end){
  return (end->tv_sec-start->tv_sec) + 1e-6 * (end->tv_usec-start->tv_usec);
}


/**
 * ritorna 0 se data è ordinato in modo crescente 1 altrimenti */
int isOrdered(int data[],int size){
	int ordinato=0;
	for(size_t i=0;i<size-1;i++){
		if(data[i]>data[i+1]){
			ordinato = 1;
			break;
		}
	}
	return ordinato;
}

void mini_test_quick_sort(){
	int MAX=6;
	int v[]={5,4,3,2,1,11};
	printf("[%s]: Vettore prima dell'ordinamento :",__func__);
  
  
	for(size_t i=0;i<MAX;i++){
		printf("[%s]: v[%ld] = %d\t :",__func__,i,v[i]);
	}
  puts("\n");
     
	quick_sort_omp_start(v,0,MAX-1);
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
  int MAX=1<<20; //2^19 elementi 
   system("cls");
    srand(time(0));
  int data[MAX];
  gen_random_numbers(data,MAX,0,2*MAX);
  int non_ordinalto = 1;
  for(int i=0;i<MAX-1;i++){
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

	merge_sort_omp_start(data,0,MAX-1);

  gettimeofday(&end, NULL);
  non_ordinalto=1;

  for(int i=0;i<MAX-1;i++){
    if(data[i]>data[i+1]){
      non_ordinalto=0;
      break;
    }
  }
  if(non_ordinalto==0)
    printf("[%s] errore con merge sort\n",__func__);
  else{

    printf("[%s] merge sort ok, tempo di esecuzione ordinamento : %0.6f effettuato con %d elementi e %d threads\n",__func__, tdiff(&start, &end),MAX,omp_get_max_threads());
	}

}

void test_big_quick_sort(){
  int MAX=1<<19; //2^19 elementi 
   system("cls");
    srand(time(0));
  int data[MAX];
  gen_random_numbers(data,MAX,0,2*MAX);
  
  int non_ordinalto = 1;
  for(int i=0;i<MAX-1;i++){
    if(data[i]>data[i+1]){
      non_ordinalto=0;
      break;
    }
  }
  if(non_ordinalto==0)
    printf("[%s] vettore di partenza disordinato\n",__func__);
  else
    printf("[%s] vettore di partenza ordinato\n",__func__);

  /*
  for(int i=0;i<MAX;i++){
    printf("[%s] v[%d]=%d\n",__func__,i,data[i]);
  }
  */
  

  struct timeval start,end;

  gettimeofday(&start, NULL);

  quick_sort_omp_start(data,0,MAX-1);

  gettimeofday(&end, NULL);

  
  non_ordinalto=1;
  for(int i=0;i<MAX-1;i++){

    if(data[i]>data[i+1]){
      non_ordinalto=0;
      break;
    }
  }
  

  if(non_ordinalto==0)
    printf("[%s] errore con quick sort\n",__func__);
  else{
    printf("[%s] quick sort ok, tempo di esecuzione ordinamento : %0.6f effettuato con %d elementi e %d threads\n",__func__, tdiff(&start, &end),MAX, omp_get_max_threads());
	}
}


/* funzione che esegue versione sequenziale e parallela del quick sort con mpi e stampa a schermo:
* dimensione array di test 
* tempo di esecuzione sequenziale 
* tempo di esecuzione parallelo 
* nr di thread usato 
* nr di cpu usato 
* speed up 
* efficienza
*/
void ben_quick_sort_mpi(){
	int SIZE = 1<<19;
	int nr_cores = omp_get_num_procs();
	int nr_threads = omp_get_max_threads();
	float execution_time_sequenzial = 0;
	float execution_time_parallel = 0;
	float speed_up = 0;
	float eff = 0;

	system("cls");
	srand(time(0));
	int data_seq[SIZE];
	int data_parall[SIZE];
	gen_random_numbers(data_seq,SIZE,0,SIZE);
	
	#pragma omp parallel for 
	for(size_t i=0;i<SIZE;i++)
		data_parall[i]=data_seq[i];


	struct timeval start,end;

	gettimeofday(&start, NULL);
	quick_sort(data_seq,0,SIZE-1);
	gettimeofday(&end, NULL);
	execution_time_sequenzial = tdiff(&start, &end);


	gettimeofday(&start, NULL);
	quick_sort_omp_start(data_parall,0,SIZE-1);
	gettimeofday(&end, NULL);
	execution_time_parallel = tdiff(&start, &end);

	speed_up = execution_time_sequenzial/execution_time_parallel;
	eff = execution_time_sequenzial /(execution_time_parallel * nr_cores);
	

  printf("[%s] Statistiche esecuzione quick sort in %s at %s\n", __func__, __DATE__, __TIME__);
  printf("dimensione array= %d\n", SIZE);
  printf("numero core = %d\n", nr_cores);
  printf("numero thread = %d\n", nr_threads);
  printf("tempo di esecuzione sequenziale %0.6f\n", execution_time_sequenzial);
  printf("tempo di esecuzione parallelo %0.6f\n", execution_time_parallel);
  printf("SPEEDUP = %f\n", speed_up);
  printf("EFFICIENZA %f\n", eff);
  printf("correttezza sequenziale %s\n",(isOrdered(data_seq, SIZE)==0 ? "OK" : "Errore"));
  printf("correttezza parallelo %s\n", (isOrdered(data_parall, SIZE)==0 ? "OK" : "Errore"));
}

/* funzione che esegue versione sequenziale e parallela del merge sort con open_mpi e stampa a schermo:
* dimensione array di test 
* tempo di esecuzione sequenziale 
* tempo di esecuzione parallelo 
* nr di thread usato 
* nr di cpu usato 
* speed up 
* efficienza
*/
void ben_merge_sort_mpi(){
	int SIZE = 1<<19;
	int nr_cores = omp_get_num_procs();
	int nr_threads = omp_get_max_threads();
	float execution_time_sequenzial = 0;
	float execution_time_parallel = 0;
	float speed_up = 0;
	float eff = 0;

	system("cls");
	srand(time(0));
	int data_seq[SIZE];
	int data_parall[SIZE];
	gen_random_numbers(data_seq,SIZE,0,SIZE);
	
	#pragma omp parallel for 
	for(size_t i=0;i<SIZE;i++)
		data_parall[i]=data_seq[i];


	struct timeval start,end;

	gettimeofday(&start, NULL);
	merge_sort(data_seq,0,SIZE-1);
	gettimeofday(&end, NULL);
	execution_time_sequenzial = tdiff(&start, &end);


	gettimeofday(&start, NULL);
	merge_sort_omp_start(data_parall,0,SIZE-1);
	gettimeofday(&end, NULL);
	execution_time_parallel = tdiff(&start, &end);

	speed_up = execution_time_sequenzial/execution_time_parallel;
	eff = execution_time_sequenzial /(execution_time_parallel * nr_cores);
	

  printf("[%s] Statistiche esecuzione merge sort in %s at %s\n", __func__, __DATE__, __TIME__);
  printf("dimensione array= %d\n", SIZE);
  printf("numero core = %d\n", nr_cores);
  printf("numero thread = %d\n", nr_threads);
  printf("tempo di esecuzione sequenziale %0.6f\n", execution_time_sequenzial);
  printf("tempo di esecuzione parallelo %0.6f\n", execution_time_parallel);
  printf("SPEEDUP = %f\n", speed_up);
  printf("EFFICIENZA %f\n", eff);
  printf("correttezza sequenziale %s\n",(isOrdered(data_seq, SIZE)==0 ? "OK" : "Errore"));
  printf("correttezza parallelo %s\n", (isOrdered(data_parall, SIZE)==0 ? "OK" : "Errore"));
}

int main(){
	 //mini_test_quick_sort();
 // mini_test_merge();

  
//  big_test_merge_sort();
//  test_big_quick_sort();
	//ben_quick_sort_mpi();
  ben_merge_sort_mpi();
  return 0;
}


