/**
 * Esperimento per provare la logica di merge tra rami e distribuzione di chunck a processi
 */
#include <math.h>
#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib.h"

//crono e thread sono usate per prendere prestazioni codice
float tdiff(struct timeval *start,struct timeval *end){
  return (end->tv_sec-start->tv_sec) + 1e-6 * (end->tv_usec-start->tv_usec);
}
// esgue il l'algoritmo di merge tra left e right
void merge_l(int *dest,size_t dest_size,int *left,size_t left_size,int *right,size_t right_size ){
  size_t i=0,j=0,k=0;
  while(i <left_size && j<right_size ){
    dest[k++]=(left[i]<=right[j])?(left[i++]):(right[j++]);
  }

  while(i<left_size){
   dest[k++]=left[i++]; 
  }

  while(j< right_size){
   dest[k++]=right[j++]; 
  }


}



/** funzione in cui testo merge parallelo senza usare struttura dati
 *
 * il sistema è descritto nel seguente modo
 *
 *      |nodo 0 a {1,5,10} -->| 
 *      |		                  | --> merge a,b{1,2,5,7,9,10} in nodo 0 --> |
 *      |nodo 1 b {2,7,9}  -->|				                                    |
 * o-->	|		                  | 			                                    | --> merge a,b a
 *	    |nodo 2 c {3,4,6} --> |				                                    | 
 *	    |		                  | --> merge c,d{3,4,6,8,11,12} in nodo 2 -->|
 *	    |nodo 3 d {8,11,12}-> |
 */



void test_merge(){

	size_t CHUNK_SIZE = 3;

	int my_rank =-1,nr_nodes=-1;

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nr_nodes);
	int *local_data = malloc(CHUNK_SIZE*sizeof(int));

		 
		if(local_data==NULL){
			printf("[%s,%d] Errore creazione data local\n",__func__,my_rank);
    exit(1);
		}

	if(my_rank==0){

		int data[]={1,5,10,2,7,9,3,4,6,8,11,12};

		printf("[%s,%d],numero di elementi data totale %ld\n",__func__,my_rank,CHUNK_SIZE*4);

    MPI_Scatter(data, (int)CHUNK_SIZE, MPI_INT, local_data, (int)CHUNK_SIZE, MPI_INT, 0, MPI_COMM_WORLD);

  }else{

    MPI_Scatter(NULL, (int)CHUNK_SIZE, MPI_INT, local_data, (int)CHUNK_SIZE, MPI_INT, 0, MPI_COMM_WORLD);
  }



		printf("[%s,%d],valore locale array:{",__func__,my_rank);
		for(size_t i=0;i<CHUNK_SIZE;i++)
			printf("%d,", local_data[i]); 
		printf("}\n");

  if(my_rank==1){
    MPI_Send(local_data,CHUNK_SIZE,MPI_INT,0,0,MPI_COMM_WORLD);
    //invio dati e mi eclisso
  }
  
  if(my_rank==3){
    //invio dati e mi eclisso
    MPI_Send(local_data,CHUNK_SIZE,MPI_INT,2,0,MPI_COMM_WORLD);
  }


  int *half=malloc(2*CHUNK_SIZE*sizeof(int));
  if(my_rank==0){
    int *tmp = malloc(CHUNK_SIZE*sizeof(int));
    if(tmp==NULL){
			printf("[%s,%d] Errore creazione tmp\n",__func__,my_rank);
    }
    MPI_Recv(tmp,CHUNK_SIZE,MPI_INT,1,0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    merge_l(half,2*CHUNK_SIZE,local_data,CHUNK_SIZE,tmp,CHUNK_SIZE);

    free(tmp);

    printf("[%s,%d] Esito merge A e B:\n",__func__,my_rank);
		for(size_t i=0;i<2*CHUNK_SIZE;i++)
			printf("%d,", half[i]); 
    printf("\n");
    printf("\n");
  }

  if(my_rank==2){
    int *tmp = malloc(CHUNK_SIZE*sizeof(int));
    if(tmp==NULL){
			printf("[%s,%d] Erroore creazione tmp\n",__func__,my_rank);
    }
    MPI_Recv(tmp,CHUNK_SIZE,MPI_INT,3,0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    merge_l(half,2*CHUNK_SIZE,local_data,CHUNK_SIZE,tmp,CHUNK_SIZE);

    free(tmp);
    printf("[%s,%d] Esito merge C e D:\n",__func__,my_rank);
		for(size_t i=0;i<2*CHUNK_SIZE;i++)
			printf("%d,", half[i]); 
    printf("\n");
  }
    
  
		free(local_data);
  MPI_Barrier(MPI_COMM_WORLD);
  // da qui in poi local_data è inutile
  if(my_rank == 2){
    MPI_Send(half,2*CHUNK_SIZE,MPI_INT,0,0,MPI_COMM_WORLD);
    //invio dati da 2 a 0
    
  }

  if(my_rank==0){
    //ricevo dati e con ultimo merge dovrei avere i dati ordinati

    int *tmp = malloc(2*CHUNK_SIZE*sizeof(int));
    int *ord = malloc(4*CHUNK_SIZE*sizeof(int));
    MPI_Recv(tmp,2*CHUNK_SIZE,MPI_INT,2,0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    merge_l(ord,4*CHUNK_SIZE,half,2*CHUNK_SIZE,tmp,2*CHUNK_SIZE);
    free(tmp);

    printf("[%s,%d] Mi aspetto di spampare i dati ordinati:\n",__func__,my_rank);
		for(size_t i=0;i<4*CHUNK_SIZE;i++)
			printf("%d,", ord[i]); 
    printf("\n");
    free(ord);
  }
  free(half);
}


void test_init(){
	int my_rank =-1,nr_nodes=-1;

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nr_nodes);
	printf("[%s,%d] Ciao sono %d e ci sono %d processi attivi\n",__func__,my_rank,my_rank,nr_nodes);


}



/**
 * funzione che implementa la strategia del binary merge tree 
 * funiziona bene quando il numero di nodi mpi è una potenza di 2
 *
 * data è un array di nodi locale di tipo int 
 * len è la lunghezza di data
 */
 
void binary_merge_tree(int **data,size_t *len){
  if(NULL == *data || NULL == data){
    printf("[%s] Errore: puntatore a dati da mergiare nullo\n",__func__);
    MPI_Abort(MPI_COMM_WORLD,EXIT_FAILURE);
  }

  if(NULL==len){
    printf("[%s] Errore: puntatore a len  nullo\n",__func__);
    MPI_Abort(MPI_COMM_WORLD,EXIT_FAILURE);
  }
  if(*len==0){
    printf("[%s] Errore: ",__func__);
    MPI_Abort(MPI_COMM_WORLD,EXIT_FAILURE);
  }

	int my_rank =-1,nr_nodes=-1;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nr_nodes);
  if( 2 > nr_nodes){
    printf("[%s] Numero di nodi troppo basso\n",__func__);
    MPI_Abort(MPI_COMM_WORLD,EXIT_FAILURE);
  }

  printf("[%s,%d],valore locale array:{",__func__,my_rank);
  for(size_t i=0;i< (*len);i++)
    printf("%d,", (*data)[i]); 
  printf("}\n");

  double exact_nr_rounds = log2((double)nr_nodes);
  size_t nr_rounds = (size_t) exact_nr_rounds;
  #ifdef DEBUG
  if(my_rank==0){
    printf("[%s,%d] Numero di round %ld\n",__func__,my_rank,nr_rounds);
  }
  #endif 
   //TODO: gestione nel caso nr nodi non log2 
  
  /*
  double tmp = exact_nr_rounds-floor(exact_nr_rounds); 
  if(tmp == 0.0) 
    nr_rounds = (size_t) exact_nr_rounds;
  */

  for(int i=1;i<=nr_rounds;i++){
      int level_chunck = 1<<i; //ovvero il numero di chuck che vengono uniti in questo round;
    if( my_rank % (level_chunck) == 0 ){ // ricezione e merge
        int dest=my_rank+ (1 << (i-1));
      if( dest < nr_nodes){
#ifdef DEBUG
        printf("[%s,%d] Round %d  ricevo dati da dest %d\n",__func__,my_rank,i,dest);
#endif 

        int* tmp = malloc(*len *sizeof(int));
        int* merged_data = malloc(2* (*len) *sizeof(int));
        MPI_Recv(tmp,*len,MPI_INT,dest,0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        merge_l(merged_data,2*(*len),*data,(*len),tmp,*len);
        free(*data);
        *data = merged_data;
        *len *= 2; 
        free(tmp);
      }
    //MPI_Barrier(MPI_COMM_WORLD);
    }
    else{
      //invio dati per merge
      int step_size_prev = 1 << (i-1);
      if(my_rank % step_size_prev == 0){
        int dest = my_rank - step_size_prev;
        if(dest >=0){
          MPI_Send(*data,*len,MPI_INT,dest,0,MPI_COMM_WORLD);
        }
      }
    }
   // MPI_Barrier(MPI_COMM_WORLD);
  }

}





void test_merge_bint(){
	size_t CHUNK_SIZE = 5;
	int my_rank =-1,nr_nodes=-1;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nr_nodes);
  size_t local_size = CHUNK_SIZE;
	int *local_data = malloc(local_size*sizeof(int));

		 
		if(local_data==NULL){
			printf("[%s,%d] Errore creazione data local\n",__func__,my_rank);
    exit(1);
		}

	if(my_rank==0){

		int data[]={1,5,10,2,7,9,3,4,6,8,11,12,13,14,15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40};

		printf("[%s,%d],numero di elementi data totale %ld\n",__func__,my_rank,CHUNK_SIZE*nr_nodes);

    MPI_Scatter(data, (int)CHUNK_SIZE, MPI_INT, local_data, (int)CHUNK_SIZE, MPI_INT, 0, MPI_COMM_WORLD);

  }else{

    MPI_Scatter(NULL, (int)CHUNK_SIZE, MPI_INT, local_data, (int)CHUNK_SIZE, MPI_INT, 0, MPI_COMM_WORLD);
  }



//		printf("[%s,%d],valore locale array:{",__func__,my_rank);
//		for(size_t i=0;i<CHUNK_SIZE;i++)
//			printf("%d,", local_data[i]); 
//		printf("}\n");

  if(my_rank==0)
		printf("[%s],Inizio merge:\n",__func__);

  binary_merge_tree(&local_data,&local_size);


    //MPI_Barrier(MPI_COMM_WORLD);
/*
  double exact_nr_rounds = log2((double)nr_nodes);
  size_t nr_rounds = (size_t) exact_nr_rounds;
  
  double exact_nr_rounds = log2((double)nr_nodes);
  size_t nr_rounds = (size_t) exact_nr_rounds;
  #ifdef DEBUG
  if(my_rank==0){
    printf("[%s,%d] Numero di round %ld\n",__func__,my_rank,nr_rounds);
  }
  #endif 
   //TODO: gestione nel caso nr nodi non log2 
  
  
  double tmp = exact_nr_rounds-floor(exact_nr_rounds); 
  if(tmp == 0.0) 
    nr_rounds = (size_t) exact_nr_rounds;

  
  for(int i=1;i<=nr_rounds;i++){

      int level_chunck = 1<<i; //ovvero il numero di chuck che vengono uniti in questo round;
      //len = level_chunck * CHUNK_SIZE;
    if( my_rank % (level_chunck) == 0 ){ // ricezione e merge
        //int dest=my_rank+i;

        int dest=my_rank+ (1 << (i-1));
      if( dest < nr_nodes){
#ifdef DEBUG
        printf("[%s,%d] Round %d  ricevo dati da dest %d\n",__func__,my_rank,i,dest);
#endif 

        int* tmp = malloc(len *sizeof(int));
        int* merged_data = malloc(2*len *sizeof(int));
        MPI_Recv(tmp,len,MPI_INT,dest,0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        merge(merged_data,2*len,local_data,len,tmp,len);
        //local_data = realloc(local_data, 2*len *sizeof(int)); //dimenticato di copiare i dati 
        free(local_data);
        local_data = merged_data;
        len *= 2; 
        //free(merged_data);
        free(tmp);

      }
    //MPI_Barrier(MPI_COMM_WORLD);
    }
    else{
      //invio dati per merge
      int step_size_prev = 1 << (i-1);
      if(my_rank % step_size_prev == 0){
        int dest = my_rank - step_size_prev;
        if(dest >=0){
          MPI_Send(local_data,len,MPI_INT,dest,0,MPI_COMM_WORLD);
        }
      }
    }

    MPI_Barrier(MPI_COMM_WORLD);
  }
  */
  

    MPI_Barrier(MPI_COMM_WORLD);
  if(my_rank==0){
    printf("[%s,%d] Array dopo merge :\n{",__func__,my_rank);
    for(size_t i=0;i<40;i++){
      printf("%d,",local_data[i]);
    }

      puts("}");

  }

  free(local_data);


}



/**
  * Funzione per verificare se ci saranno problemi nell'invio di dati con la scatter 
  * len la lunghezza de dati originali,
  * chunk_size la dimensione del chunk attuale 
  *
  * la funzione restituisce:
  * 0 se i dati saranno distribuiti equamente 
  * 1 se il chunk è piccolo 
  * 2 se il chunk è troppo grande 
  * -1 in caso di problemi con i parametri o il numero di nodi
*/

int check_chunks (size_t len,size_t chunk_size){
	int nr_nodes=-1;
	MPI_Comm_size(MPI_COMM_WORLD, &nr_nodes);

  if( 0 == len ){
    printf("[%s] errore con campo len: ha valore 0\n",__func__);
    return -1;
  }

  if( 0 == chunk_size){
    printf("[%s] errore con campo chunk_size: ha valore 0\n",__func__);
    return -1;
  }

  if( 1 == chunk_size){ // caso banale 
    return 0;
  }

  if( 1 > nr_nodes ){
    printf("[%s] problemi con prelevamento numero di nodi, numero di nodi trovato %d \n",__func__,nr_nodes);
    return -1;
  }

  if( len < nr_nodes ){
    printf("[%s] numero di nodi troppo grande rispetto alla dimensione dei dati iniziali \n",__func__);
    return -1;
  }

  int ideal_chunk_size = len/nr_nodes;

  if(chunk_size == ideal_chunk_size)
    return 0;

  if(chunk_size > ideal_chunk_size)
    return 2;

  return 1;


  
}

/** main di funzione in cui provo a capire come rilevare errori della scatter */
void find_error_scatter(){
// caso in cui i riesco ad assegnare il numero ideale di chunk a ogni processo

	size_t CHUNK_SIZE = 2;
	int my_rank =-1,nr_nodes=-1;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nr_nodes);
	int *local_data = malloc(CHUNK_SIZE*sizeof(int));

	if(my_rank==0){

		int data[]={1,5,10,2,7,9,3,4,6,8,11,12,13,14,15,16};

		printf("[%s,%d],numero di elementi data totale %ld : Caso distribuzione corretta\n",__func__,my_rank,CHUNK_SIZE*4);

    MPI_Scatter(data, (int)CHUNK_SIZE, MPI_INT, local_data, (int)CHUNK_SIZE, MPI_INT, 0, MPI_COMM_WORLD);

  }else{

    MPI_Scatter(NULL, (int)CHUNK_SIZE, MPI_INT, local_data, (int)CHUNK_SIZE, MPI_INT, 0, MPI_COMM_WORLD);
  }

		printf("[%s,%d],valore locale array:{",__func__,my_rank);
		for(size_t i=0;i<CHUNK_SIZE;i++)
			printf("%d,", local_data[i]); 
		printf("}\n");
  free(local_data);
/*
  MPI_Barrier(MPI_COMM_WORLD);

  // caso in cui chunk troppo grande 
  CHUNK_SIZE = 5;
  *local_data = malloc(CHUNK_SIZE*sizeof(int));

	if(my_rank==0){

		int data[]={1,5,10,2,7,9,3,4,6,8,11,12,13,14,15,16};

		printf("[%s,%d],numero di elementi data totale %ld. Caso in cui chunk troppo grandi\n",__func__,my_rank,CHUNK_SIZE*4);

    MPI_Scatter(data, (int)CHUNK_SIZE, MPI_INT, local_data, (int)CHUNK_SIZE, MPI_INT, 0, MPI_COMM_WORLD);

  }else{

    MPI_Scatter(NULL, (int)CHUNK_SIZE, MPI_INT, local_data, (int)CHUNK_SIZE, MPI_INT, 0, MPI_COMM_WORLD);
  }

		printf("[%s,%d],valore locale array:{",__func__,my_rank);
		for(size_t i=0;i<CHUNK_SIZE;i++)
			printf("%d,", local_data[i]); 
		printf("}\n");
  free(local_data);

  
  MPI_Barrier(MPI_COMM_WORLD);
  // caso in cui chunck troppo piccolo
  
  CHUNK_SIZE = 1;
  local_data = malloc(CHUNK_SIZE*sizeof(int));

	if(my_rank==0){

		int data[]={1,5,10,2,7,9,3,4,6,8,11,12,13,14,15,16};

		printf("[%s,%d],numero di elementi data totale %ld. Caso in cui chunk troppo piccoli\n",__func__,my_rank,CHUNK_SIZE*4);

    MPI_Scatter(data, (int)CHUNK_SIZE, MPI_INT, local_data, (int)CHUNK_SIZE, MPI_INT, 0, MPI_COMM_WORLD);

  }else{

    MPI_Scatter(NULL, (int)CHUNK_SIZE, MPI_INT, local_data, (int)CHUNK_SIZE, MPI_INT, 0, MPI_COMM_WORLD);
  }

		printf("[%s,%d],valore locale array:{",__func__,my_rank);
		for(size_t i=0;i<CHUNK_SIZE;i++)
			printf("%d,", local_data[i]); 
		printf("}\n");
  free(local_data);
  */
}

//void merge_sort_mpi(int *data,size_t len){}

void ben_merge_sort(){

}
/**
  * primo uso della scatter v 
* in questo test uso 3 nodi mpi e un vettore di lunghezza 10 i dati vengono assegnati nel seguente modo:
* - rank 0 range 0,3  len(4)
* - rank 1 range 4,7  len(3)
* - rank 2 range 8,9  len(2)
*/
void test_scatterv() {
    int my_rank = -1, nr_nodes = -1;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nr_nodes);

    if (nr_nodes != 3) {
        printf("[%s] Errore: per questo test con la scatterv si usano solo 3 nodi mpi\n", __func__);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    int sizes[3]  = {4, 3, 2};
    int displs[3] = {0, 4, 7};  // offset cumulativi

    int *local_data = malloc(sizes[my_rank] * sizeof(int));
    if (local_data == NULL) {
        printf("[%s,%d] Problema assegnazione local_data\n", __func__, my_rank);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    int *data = NULL;
    if (my_rank == 0) {
        int size = sizes[0] + sizes[1] + sizes[2]; // = 9
        data = malloc(size * sizeof(int));
        for (int i = 0; i < size; i++) {
            data[i] = i;
        }
    }

    MPI_Scatterv(data, sizes, displs, MPI_INT,
                 local_data, sizes[my_rank], MPI_INT,
                 0, MPI_COMM_WORLD);

    printf("[%s,%d] Dati locali: {", __func__, my_rank);
    for (int i = 0; i < sizes[my_rank]; i++) {
        printf("%d,", local_data[i]);
    }
    printf(" }\n");

    free(local_data);
    if (my_rank == 0) free(data);
}

/**
 * La funzione MPI_Alltoall è una combinazione efficiente di scatter e gather e permette un efficiente scambio di messaggi 
 * per questo scenario di test uso 3 processi che possiedono i seguenti dati:
 * - rank 0: [0,100,200]
 * - rank 1: [300,400,500]
 * - rank 2: [600,700,800]
 * Dopo la all_to_all ogni processo riceve i seguenti dati:
 * - rank 0: [0,300,600]
 * - rank 1: [100,400,700]
 * - rank 2: [200,500,800]
 */
void test_all_to_all(){
    int my_rank = -1, nr_nodes = -1;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nr_nodes);
    if (nr_nodes != 3) {
        printf("[%s] Errore: per questo test con la scatterv si usano solo 3 nodi mpi\n", __func__);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }
    int local_data[3];
    int recv_data[3];
  for(size_t i=0;i<3;i++){
    local_data[i] = my_rank * 300 + i*100;
  }
  MPI_Barrier(MPI_COMM_WORLD);
  printf("[%s,%d] Valori locali iniziali:\n{ ",__func__,my_rank);
    for(size_t i=0;i<3;i++){
      printf("%d,\t",local_data[i]);
    }
  printf("}\n");
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Alltoall(&local_data,3,MPI_INT,recv_data,1,MPI_INT,MPI_COMM_WORLD);

  printf("[%s,%d] Valori scambiati finali:\n{ ",__func__,my_rank);
    for(size_t i=0;i<3;i++){
      printf("%d,\t",recv_data[i]);
    }
  printf("}\n");

}
/* funzione in cui uso per la prima volta una virtual topologia con mpi 
 * la topologia presenta un grafo con 8 nodi con la seguente struttura:
 *                     |
 *               -------------
 *              /            \
 *            /               \
 *         -------        --------
 *        /      \       /       \
 *      ---     ---     ---     ---
 *     /  \    /  \    /  \    /  \ 
 *   |0| |1| |2| |3| |4| |5| |6| |7|
 *
 *   quindi:
 *   - il nodo 0 ha collegamenti con 1,2,4 
 *   - il nodo 1 ha collegamento solo con 1
 *   - il nodo 2 ha collegamenti con 0 e 3 
 *   - il nodo 4 ha collegamenti con 0 e 5
 *   - il nodo 5 ha collegamento solo con 4
 *   - il nodo 6 ha collegamenti con 4 e 7
 *   - il nodo 7 ha collegamento solo con 6
 
	*/
void test_graph(){printf("lol"); }
/*
  * versione di merge sort che utilizza la libreria mpi per funzionare , distribuisce i dati in chuks a vari nodi mpi che eseguono ordinamento locale 
  * ed eseguono un merge dei chunk ordinati usando la strategia del binary_merge_tree .
  * data dati da ordinare 
  * len la lunghezza dei dati da ordinare 
         */
void merge_sort_mpi(int *data,size_t len){
	int my_rank =-1,nr_nodes=-1;
   //int CHUNK_SIZE = 1<<7;
   int CHUNK_SIZE = 8192;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nr_nodes);
  if( NULL == data && my_rank == 0 ){
    printf("[%s] Errore puntatore a data null\n",__func__);
    MPI_Abort(MPI_COMM_WORLD,EXIT_FAILURE);
  }
  if( 0 == len ){
    printf("[%s] Errore con la lunghezza indicata in len\n",__func__);
    MPI_Abort(MPI_COMM_WORLD,EXIT_FAILURE);
  }
  if(  2 > nr_nodes  ){
      printf("[%s] Errore: numero di nodi mpi  troppo basso.\n",__func__);
      MPI_Abort(MPI_COMM_WORLD,EXIT_FAILURE);
    }
  if( len <= CHUNK_SIZE ){
    merge_sort(data,0,len-1);
    return;
  }
  #ifdef DEBUG
  printf("[%s] Ho passato i controlli preliminari e sto per avviare la distribuzione dei dati\n",__func__);
  #endif 

  int *local_data = malloc(CHUNK_SIZE * sizeof(int));

  if( NULL == local_data){
      printf("[%s,%d] Errore: problemi allocazione local data.\n",__func__,my_rank);
      MPI_Abort(MPI_COMM_WORLD,EXIT_FAILURE);
  }
  // suddivisione in chunk dei dati
  if( my_rank == 0 ){
    MPI_Scatter(data, (int)CHUNK_SIZE, MPI_INT, local_data, (int)CHUNK_SIZE, MPI_INT, 0, MPI_COMM_WORLD);
  }else{
    MPI_Scatter(NULL, (int)CHUNK_SIZE, MPI_INT, local_data, (int)CHUNK_SIZE, MPI_INT, 0, MPI_COMM_WORLD);
  }

  // ordinamento dati locali 
  
  size_t local_size = CHUNK_SIZE;

  #ifdef DEBUG
  printf("[%s,%d] Sto lanciando il sort locale sui chunk con local size %ld \n",__func__,my_rank,local_size);
  #endif 
  merge_sort(local_data,0,local_size-1);

  // merge
  MPI_Barrier(MPI_COMM_WORLD);

  #ifdef DEBUG
  printf("[%s] lancio il binary merge tree\n",__func__);
  #endif 
  binary_merge_tree(&local_data,&local_size);
  MPI_Barrier(MPI_COMM_WORLD);

  #ifdef DEBUG
  printf("[%s] Copio i dati locali in data da local data\n",__func__);
  #endif 
  if( my_rank == 0 ){
    if( local_size != len ){
      printf("[%s,%d] Errore dopo il merge le lunghezze di len e local len non corrispondono\n",__func__,my_rank);
      MPI_Abort(MPI_COMM_WORLD,EXIT_FAILURE);
    }else{
      memcpy(data, local_data, local_size * sizeof(int));
    }
  }
  free(local_data);
}

/* verifica se data è in oridne crescente e restiusce 0 in caso affermativo, se almeno un elemento non è ordinato restiuisce 1*/
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

void gen_random_numbers(int *array, int len, int min, int max){
    for (int i = 0; i < len; i++)
        array[i] = rand() % (max - min + 1) + min;
}
/** funzione di bechmark dove provo a vedere se funzione la funzione di merge e con quali prestazioni
*/
void ben_merge_sort_mpi() {
    int my_rank = -1, nr_nodes = -1;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nr_nodes);
    
    //int SIZE = 40;
    int SIZE = 1<<16;
    int nr_cores = omp_get_num_procs();
    int nr_threads = omp_get_max_threads();
    float execution_time_sequenzial = 0;
    float execution_time_parallel = 0;
    float speed_up = 0;
    float eff = 0;
    
    int *data_seq = NULL;
    int *data_parall = NULL;
    
    // Solo rank 0 alloca e inizializza
    if (my_rank == 0) {
        srand(time(0));
        data_seq = malloc(SIZE * sizeof(int));
        data_parall = malloc(SIZE * sizeof(int));
        
        if (data_seq == NULL) {
            printf("[%s,%d] Errore creazione array data seq di %d elementi\n",
                   __func__, my_rank, SIZE);
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        }
        if (data_parall == NULL) {
            printf("[%s,%d] Errore creazione array data parall di %d elementi\n",
                   __func__, my_rank, SIZE);
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        }
        
        gen_random_numbers(data_seq, SIZE, 0, SIZE);
        
        #pragma omp parallel for 
        for (size_t i = 0; i < SIZE; i++)
            data_parall[i] = data_seq[i];
        
        // Ordinamento sequenziale (solo rank 0)
        struct timeval start, end;
        gettimeofday(&start, NULL);
        merge_sort(data_seq, 0,SIZE - 1);
        gettimeofday(&end, NULL);
        execution_time_sequenzial = tdiff(&start, &end);
        
#ifdef DEBUG
        printf("[%s] Ordinamento sequenziale ok\n", __func__);
        printf("[%s] Array prima di MPI: ", __func__);
        for(int i = 0; i < (SIZE < 20 ? SIZE : 20); i++)
            printf("%d ", data_parall[i]);
        printf("%s\n", SIZE > 20 ? "..." : "");
#endif
    }
    
    // Sincronizza tutti prima del timing parallelo
    MPI_Barrier(MPI_COMM_WORLD);
    
    struct timeval start, end;
    if (my_rank == 0) {
        gettimeofday(&start, NULL);
    }
    
    // *** TUTTI i processi chiamano merge_sort_mpi ***
    merge_sort_mpi(data_parall, SIZE);
    
    if (my_rank == 0) {
        gettimeofday(&end, NULL);
        execution_time_parallel = tdiff(&start, &end);
        
        speed_up = execution_time_sequenzial / execution_time_parallel;
        eff = execution_time_sequenzial / (execution_time_parallel * nr_nodes);
        
        printf("[%s] Statistiche esecuzione merge sort mpi in %s at %s\n", 
               __func__, __DATE__, __TIME__);
        printf("dimensione array= %d\n", SIZE);
        printf("numero nodi MPI = %d\n", nr_nodes);
        printf("numero core = %d\n", nr_cores);
        printf("numero thread = %d\n", nr_threads);
        printf("tempo di esecuzione sequenziale %0.6f\n", execution_time_sequenzial);
        printf("tempo di esecuzione parallelo MPI %0.6f\n", execution_time_parallel);
        printf("SPEEDUP = %0.2f\n", speed_up);
        printf("EFFICIENZA = %0.2f\n", eff);
        printf("correttezza sequenziale %s\n", 
               (isOrdered(data_seq, SIZE) == 0 ? "OK" : "Errore"));
        printf("correttezza parallelo %s\n", 
               (isOrdered(data_parall, SIZE) == 0 ? "OK" : "Errore"));
        
#ifdef DEBUG
        printf("[%s] Array dopo MPI: ", __func__);
        for(int i = 0; i < (SIZE < 20 ? SIZE : 20); i++)
            printf("%d ", data_parall[i]);
        printf("%s\n", SIZE > 20 ? "..." : "");
#endif
        
        free(data_seq);
        free(data_parall);
    }
}
int main(int argc,char *argv[]){
  MPI_Init(&argc, &argv);
  //test_init();
  //test_merge();
  //excevive_data();
  //test_merge_bint();
  //find_error_scatter();
  //test_scatterv();
  //test_all_to_all();
  ben_merge_sort_mpi();
  MPI_Finalize();
  return 0;

}
