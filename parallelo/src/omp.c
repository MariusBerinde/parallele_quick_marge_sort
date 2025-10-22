#include "lib.h"

//crono e thread sono usate per prendere prestazioni codice
float tdiff(struct timeval *start,struct timeval *end){
  return (end->tv_sec-start->tv_sec) + 1e-6 * (end->tv_usec-start->tv_usec);
}


/**
 * ritorna 0 se data è ordinato in modo crescente 1 altrimenti */
int isOrdered(long data[],size_t size){
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
  long v[]={5,4,3,2,1,11};
  printf("[%s]: Vettore prima dell'ordinamento :",__func__);


  for(size_t i=0;i<MAX;i++){
    printf("[%s]: v[%ld] = %ld\t :",__func__,i,v[i]);
  }
  puts("\n");

  	//quick_sort_omp_start(v,0,MAX-1,MEDIAN_ACTIVATION);
//  	quick_sort(v,0,MAX-1);
  median_quick_sort(v,0,MAX-1);
  printf("[%s]: Vettore dopo oridinamento \n",__func__);
  for(size_t i=0;i<MAX;i++){
    printf("[%s]: v[%ld] = %ld\n :",__func__,i,v[i]);
  }

}

void mini_test_merge(){
  long v[]={38,27,43,3,9,82,10};
  int MAX=7;

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




/**
	* funzione per testare tempi di sviluppo con 2^19 elementi
*/

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
	size_t SIZE = 1<<25;
	int nr_cores = omp_get_num_procs();
	int nr_threads = omp_get_max_threads();
	float execution_time_sequenzial = 0;
	float execution_time_parallel = 0;
	float speed_up = 0;
	float eff = 0;

	srand(time(0));
	long *data_seq = malloc(SIZE * sizeof(long));
	long *data_parall= malloc(SIZE * sizeof(long));
	gen_random_numbers(data_seq,SIZE,0,SIZE);
	
	#pragma omp parallel for 
	for(size_t i=0;i<SIZE;i++)
		data_parall[i]=data_seq[i];


	struct timeval start,end;

	gettimeofday(&start, NULL);
//	quick_sort(data_seq,0,SIZE-1);
	median_quick_sort(data_seq,0,SIZE-1);
	gettimeofday(&end, NULL);
	execution_time_sequenzial = tdiff(&start, &end);


	gettimeofday(&start, NULL);
	quick_sort_omp_start(data_parall,0,SIZE-1,MEDIAN_ACTIVATION);
	gettimeofday(&end, NULL);
	execution_time_parallel = tdiff(&start, &end);

	speed_up = execution_time_sequenzial/execution_time_parallel;
	eff = execution_time_sequenzial /(execution_time_parallel * nr_cores);
	

  printf("[%s] Statistiche esecuzione quick sort in %s at %s\n", __func__, __DATE__, __TIME__);
  printf("dimensione array= %ld\n", SIZE);
  printf("numero core = %d\n", nr_cores);
  printf("numero thread = %d\n", nr_threads);
  printf("tempo di esecuzione sequenziale %0.6f\n", execution_time_sequenzial);
  printf("tempo di esecuzione parallelo %0.6f\n", execution_time_parallel);
  printf("SPEEDUP = %f\n", speed_up);
  printf("EFFICIENZA %f\n", eff);
  printf("correttezza sequenziale %s\n",(isOrdered(data_seq, SIZE)==0 ? "OK" : "Errore"));
  printf("correttezza parallelo %s\n", (isOrdered(data_parall, SIZE)==0 ? "OK" : "Errore"));

	free(data_seq);
	free(data_parall);
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
	size_t SIZE = 1<<20;
	int nr_cores = omp_get_num_procs();
	int nr_threads = omp_get_max_threads();
	float execution_time_sequenzial = 0;
	float execution_time_parallel = 0;
	float speed_up = 0;
	float eff = 0;

	srand(time(0));
	long *data_seq = malloc(SIZE * sizeof(long));
	long *data_parall= malloc(SIZE * sizeof(long));
	if(data_seq==NULL){
//		perror("[%s] Errore creazione array data seq di %d element arresto forzato\n",__func__,SIZE);

		printf("[%s] Errore creazione array data seq di %ld element arresto forzato\n",__func__,SIZE);
		exit(1);
	}

	if(data_parall==NULL){
		printf("[%s] Errore creazione array data parall di %ld element arresto forzato\n",__func__,SIZE);
		exit(1);
	}
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
  printf("dimensione array= %ld\n", SIZE);
  printf("numero core = %d\n", nr_cores);
  printf("numero thread = %d\n", nr_threads);
  printf("tempo di esecuzione sequenziale %0.6f\n", execution_time_sequenzial);
  printf("tempo di esecuzione parallelo %0.6f\n", execution_time_parallel);
  printf("SPEEDUP = %0.2f\n", speed_up);
  printf("EFFICIENZA %0.2f\n", eff);
  printf("correttezza sequenziale %s\n",(isOrdered(data_seq, SIZE)==0 ? "OK" : "Errore"));
  printf("correttezza parallelo %s\n", (isOrdered(data_parall, SIZE)==0 ? "OK" : "Errore"));
	
	free(data_seq);
	free(data_parall);
}

void printArray(long *data,size_t LEN){
  printf("[%s] stampa array:{\t",__func__);
  size_t i;
  for( i=0;i<LEN-1;i++)
    printf("%ld,",data[i]);
  printf("%ld }\n",data[i]);

}
void test_random(){
  size_t SIZE = 1<<28;
  long *a = malloc(SIZE*sizeof(long));
  long *b = malloc(SIZE*sizeof(long));
  if( NULL == a){
    printf("[%s] errore allocazione a\n",__func__);
    exit(EXIT_FAILURE);
  }

  if( NULL == b){
    printf("[%s] errore allocazione b",__func__);
    exit(EXIT_FAILURE);
  }

	struct timeval start_s,end_s;
	struct timeval start_p,end_p;

	gettimeofday(&start_s, NULL);
  gen_random_numbers(a,SIZE,0,SIZE);
	gettimeofday(&end_s, NULL);

	gettimeofday(&start_p, NULL);
  gen_random_numbers(b,SIZE,0,SIZE);
	gettimeofday(&end_p, NULL);
//  printArray(a,SIZE);
 // printArray(b,SIZE);
//  printf("[%s] a[0]==b[0] esito %d",__func__,(a[0]==b[0]));
  int all_eq = 1;
  for(size_t i=0;i<SIZE;i++){
    if(a[i] != b[i]){
      printf("[%s] ERRORE RANDOM NON DETERMINISTICO: in posizione %ld a=%ld e b=%ld\n",__func__,i,a[i],b[i]);
      all_eq = !all_eq;
      break;
    }
  }
  if(all_eq){
    printf("[%s] a e b sono uguali\n",__func__);
  }

  printf("[%s] tempo di generazione %ld numeri casuali in versione sequenziale %0.6f secondi e in versione parallela %0.6f secondi\n",__func__,SIZE,tdiff(&start_s, &end_s),tdiff(&start_p, &end_p));


  free(a);
  free(b);
}

int main(){
  test_random();
// mini_test_quick_sort();
//  mini_test_merge();
//  	ben_quick_sort_mpi();
  ben_merge_sort_mpi();
  return 0;
}
