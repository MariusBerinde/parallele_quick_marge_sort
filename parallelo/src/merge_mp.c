/**
 * Esperimento per provare la logica di merge tra rami e distribuzione di chunck a processi
 */
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
// esgue il l'algoritmo di merge tra left e right
void merge(int *dest,size_t dest_size,int *left,size_t left_size,int *right,size_t right_size ){
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

    merge(half,2*CHUNK_SIZE,local_data,CHUNK_SIZE,tmp,CHUNK_SIZE);

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

    merge(half,2*CHUNK_SIZE,local_data,CHUNK_SIZE,tmp,CHUNK_SIZE);

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
    merge(ord,4*CHUNK_SIZE,half,2*CHUNK_SIZE,tmp,2*CHUNK_SIZE);
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

        merge(merged_data,2*(*len),*data,(*len),tmp,*len);
        //data = realloc(data, 2*len *sizeof(int)); //dimenticato di copiare i dati 
        free(*data);
        *data = merged_data;
        *len *= 2; 
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
          MPI_Send(*data,*len,MPI_INT,dest,0,MPI_COMM_WORLD);
        }
      }
    }
    MPI_Barrier(MPI_COMM_WORLD);
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


    MPI_Barrier(MPI_COMM_WORLD);
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
  * test per osservare comportamento di scatter con troppi dati 
*/

int main(int argc,char *argv[]){
  MPI_Init(&argc, &argv);
//  test_init();
  //test_merge();
  //excevive_data();
  test_merge_bint();
  MPI_Finalize();
  return 0;

}
