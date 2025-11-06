/**
 * Esperimento per provare la logica di merge tra rami e distribuzione di chunck a processi
 */
#include <math.h>
#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib_int.h"
#define MAX_MPI_CHUNK (4294963200)  
#define MAX_MPI_ELEMENTS (MAX_MPI_CHUNK / sizeof(int))  // ~536M elementi int
//crono e thread sono usate per prendere prestazioni codice
float tdiff(struct timeval *start,struct timeval *end){
  return (end->tv_sec-start->tv_sec) + 1e-6 * (end->tv_usec-start->tv_usec);
}

//Algoritmo di merge locale dei dati 
void merge_l(int *dest,size_t dest_size,int *left,size_t left_size,int *right,size_t right_size ){
  size_t i=0,j=0,k=0;
  while(i <left_size && j<right_size ){
    dest[k++]=(left[i]<=right[j])?(left[i++]):(right[j++]);
  }

  
  if(i<left_size){
    memcpy(&dest[k], &left[i], (left_size - i) * sizeof(int));
    k += (left_size - i);
  }
  
  if(j<right_size){
    memcpy(&dest[k], &right[j], (right_size - j) * sizeof(int));
    k += (right_size - j);
  }
  
}


void printArray(int *data,size_t LEN){
  printf("[%s] stampa array:{\t",__func__);
  size_t i;
  for( i=0;i<LEN-1;i++)
    printf("%d,",data[i]);
  printf("%d }\n",data[i]);
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
    MPI_Abort(MPI_COMM_WORLD,EXIT_FAILURE);
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
      MPI_Abort(MPI_COMM_WORLD,EXIT_FAILURE);
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
      MPI_Abort(MPI_COMM_WORLD,EXIT_FAILURE);
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
  // da qui in poi local_data Ã¨ inutile
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



void binary_merge_tree_old(int **data,size_t *len){

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

  
//  printf("[%s,%d],valore locale array:{",__func__,my_rank);
//  for(size_t i=0;i< (*len);i++)
//    printf("%d,", (*data)[i]); 
//  printf("}\n");

  double exact_nr_rounds = log2((double)nr_nodes);
  size_t nr_rounds = (size_t) exact_nr_rounds;
  #ifdef DEBUG
  if(my_rank==0){
    printf("[%s,%d] Numero di round %ld\n",__func__,my_rank,nr_rounds);
  }
  #endif 
   //TODO: gestione nel caso nr nodi non log2 
  
  
  

  for(int i=1;i<=nr_rounds;i++){
      int level_chunck = 1<<i; //ovvero il numero di chuck che vengono uniti in questo round;
    if( my_rank % (level_chunck) == 0 ){ // ricezione e merge
        int dest=my_rank+ (1 << (i-1));
      if( dest < nr_nodes){
#ifdef DEBUG
        printf("[%s,%d] Round %d  ricevo dati da dest %d\n",__func__,my_rank,i,dest);
#endif 

        int* tmp = malloc(*len *sizeof(int));
        size_t size_merged_data = 2* (*len);
        int* merged_data = malloc(size_merged_data *sizeof(int));
        if( NULL == merged_data){
          printf("[%s,%d] errore allocazione spazio merged data\n",__func__,my_rank);
          MPI_Abort(MPI_COMM_WORLD,EXIT_FAILURE);
        }
        MPI_Recv(tmp,*len,MPI_INT,dest,0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        merge_l(merged_data,size_merged_data,*data,(*len),tmp,*len);
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

      #ifdef DEBUG 
        printf("[%s,%d] invio verso %d\n",__func__,my_rank,dest);
      #endif
        if(dest >=0){
          MPI_Send(*data,*len,MPI_INT,dest,0,MPI_COMM_WORLD);
        }
      }
    }
  }

    
}





/**
 * Invia array di dimensione arbitraria superando il limite MPI
 */

void send_large_array(int *data, size_t total_size, int dest, int tag, MPI_Comm comm) {
  
    size_t remaining = total_size;
    size_t offset = 0;
    int chunk_id = 0;

    #ifdef DEBUG
    printf("[send_large_array] Invio %zu elementi (%.2f GB) a rank %d\n",
           total_size, (total_size * sizeof(int)) / (1024.0*1024.0*1024.0), dest);
    #endif

    // Invia dimensione totale prima
    MPI_Send(&total_size, 1, MPI_UNSIGNED_LONG, dest, tag, comm);

    // Invia i dati in chunk
    while (remaining > 0) {
        // CORREZIONE: confronta ELEMENTI con MAX_MPI_ELEMENTS
        size_t to_send = (remaining > MAX_MPI_ELEMENTS) ? MAX_MPI_ELEMENTS : remaining;

        #ifdef DEBUG
        if (chunk_id == 0 || remaining <= MAX_MPI_ELEMENTS) {
            printf("[send_large_array] Chunk %d: invio %zu elementi (%.2f MB)\n",
                   chunk_id, to_send, (to_send * sizeof(int)) / (1024.0*1024.0));
        }
        #endif

        MPI_Send(&data[offset], to_send, MPI_INT, dest, tag + chunk_id + 1, comm);

        offset += to_send;
        remaining -= to_send;
        chunk_id++;
    }

    #ifdef DEBUG
    printf("[send_large_array] Invio completato: %d chunk inviati\n", chunk_id);
    #endif
}

/**
 * Riceve array di dimensione arbitraria superando il limite MPI
 */
void recv_large_array(int *data, size_t expected_size, int source, int tag, MPI_Comm comm) {
    size_t total_size;

    // Ricevi dimensione totale
    MPI_Recv(&total_size, 1, MPI_UNSIGNED_LONG, source, tag, comm, MPI_STATUS_IGNORE);

    #ifdef DEBUG
    printf("[recv_large_array] Ricevo %zu elementi (%.2f GB) da rank %d\n",
           total_size, (total_size * sizeof(int)) / (1024.0*1024.0*1024.0), source);
    #endif

    if (total_size != expected_size) {
        printf("ERRORE: dimensione ricevuta (%zu) != attesa (%zu)\n",
               total_size, expected_size);
        MPI_Abort(comm, EXIT_FAILURE);
    }

    // Ricevi i dati in chunk
    size_t remaining = total_size;
    size_t offset = 0;
    int chunk_id = 0;

    while (remaining > 0) {
        // CORREZIONE: confronta ELEMENTI con MAX_MPI_ELEMENTS
        size_t to_recv = (remaining > MAX_MPI_ELEMENTS) ? MAX_MPI_ELEMENTS : remaining;

        #ifdef DEBUG
        if (chunk_id == 0 || remaining <= MAX_MPI_ELEMENTS) {
            printf("[recv_large_array] Chunk %d: ricevo %zu elementi (%.2f MB)\n",
                   chunk_id, to_recv, (to_recv * sizeof(int)) / (1024.0*1024.0));
        }
        #endif

        MPI_Recv(&data[offset], to_recv, MPI_INT, source, tag + chunk_id + 1,
                 comm, MPI_STATUS_IGNORE);

        offset += to_recv;
        remaining -= to_recv;
        chunk_id++;
    }

    #ifdef DEBUG
    printf("[recv_large_array] Ricezione completata: %d chunk ricevuti\n", chunk_id);
    #endif
}


void merge_opt(int *data,size_t len,size_t old_len,int *right,size_t len_rig){
  if( NULL == data ){
    printf("[%s] error data null\n",__func__);
    exit(EXIT_FAILURE);
  }

  if( NULL == right){
    printf("[%s] error right null\n",__func__);
    exit(EXIT_FAILURE);
  }

  size_t i=0,j=old_len,k=0;
  while(j<len && k<len_rig){
    data[i++] = (data[j]<=right[k])?(data[j++]):(right[k++]);
  }

  if(j<len){
    memcpy(&data[i], &data[j], (len - j) * sizeof(int));
    i += (len-j);
  }
  if(k<len_rig){
    memcpy(&data[i], &right[k], (len_rig - k) * sizeof(int));
    i += (len_rig - k);
  }

}
 

void binary_merge_tree(int **data, size_t *len) {
  if (NULL == *data || NULL == data) {
    printf("[%s] Errore: puntatore a dati da mergiare nullo\n", __func__);
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }
  if (NULL == len) {
    printf("[%s] Errore: puntatore a len nullo\n", __func__);
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }
  if (*len == 0) {
    printf("[%s] Errore: lunghezza array nulla\n", __func__);
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }
  
  int my_rank = -1, nr_nodes = -1;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nr_nodes);
  
  if (2 > nr_nodes) {
    printf("[%s] Numero di nodi troppo basso\n", __func__);
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }

  // Calcola l'upper bound del numero massimo di turni 
  int max_rounds = 0;
  int temp = nr_nodes - 1;
  while (temp > 0) {
    max_rounds++;
    temp >>= 1; // modo efficiente di calcolare la potenza di 2 utile
  }

#ifdef DEBUG
  if (my_rank == 0) {
    printf("[%s,%d] Numero massimo di round: %d per %d nodi\n", __func__, my_rank, max_rounds, nr_nodes);
  }
#endif

  // Array per tracciare quali nodi sono ancora attivi
  int active = 1;
  
  // Ciclo principale
  for (int round = 0; round < max_rounds; round++) {
    int step = 1 << round;
    int group_size = step << 1;
    
    #ifdef DEBUG
    if (0 == my_rank) {
      printf("[%s,%d] round %d\t valore step %d\tvalore group size %d\n",
             __func__, my_rank, round, step, group_size);
    }
    #endif
    
    if (!active) {
      continue;
    }
    
    // Determina il ruolo del nodo in questo round
    int group_id = my_rank / group_size;
    int pos_in_group = my_rank % group_size;
    
    if (pos_in_group < step) {
      // Questo nodo dovrebbe ricevere (se esiste un partner)
      int partner = my_rank + step;
      
      if (partner < nr_nodes) {
        // Il partner esiste, ricevi i dati
        
        // Prima ricevi la dimensione dei dati in arrivo
        size_t incoming_size = 0;
        MPI_Recv(&incoming_size, 1, MPI_UNSIGNED_LONG, partner, round * 1000, 
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
#ifdef DEBUG
        printf("[%s,%d] Round %d: ricevo %zu elementi da rank %d (%.2f GB)\n", __func__, my_rank, round, incoming_size, partner,
               (incoming_size * sizeof(int)) / (1024.0*1024.0*1024.0));
#endif
        
        // Alloca spazio per i dati in arrivo
        int *tmp = malloc(incoming_size * sizeof(int));
        if (NULL == tmp) {
          printf("[%s,%d] Errore allocazione tmp (%zu elementi, %.2f GB)\n",
                 __func__, my_rank, incoming_size,
                 (incoming_size * sizeof(int)) / (1024.0*1024.0*1024.0));
          MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        }
        
        // Ricevi i dati usando recv_large_array
        recv_large_array(tmp, incoming_size, partner, round * 1000 + 1, MPI_COMM_WORLD);
        
        // Alloca spazio per il merge
        size_t new_size = *len + incoming_size;
        int *merged_data = malloc(new_size * sizeof(int));
        if (NULL == merged_data) {
          printf("[%s,%d] Errore allocazione merged_data (%zu elementi, %.2f GB)\n",
                 __func__, my_rank, new_size,
                 (new_size * sizeof(int)) / (1024.0*1024.0*1024.0));
          MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        }
        
        // Esegui il merge
        merge_l(merged_data, new_size, *data, *len, tmp, incoming_size);
        
        // Sostituisci i dati vecchi con quelli mergiati
        free(*data);
        *data = merged_data;
        *len = new_size;
        free(tmp);
        
      }
      
    } else {
      // Questo nodo dovrebbe inviare
      int partner = my_rank - step;
      
#ifdef DEBUG
      printf("[%s,%d] Round %d: invio %zu elementi a nodo %d (%.2f GB)\n", __func__, my_rank, round, *len, partner, (*len * sizeof(int)) / (1024.0*1024.0*1024.0));
#endif

      // Invia prima la dimensione
      MPI_Send(len, 1, MPI_UNSIGNED_LONG, partner, round * 1000, MPI_COMM_WORLD);
      
      // Poi invia i dati usando send_large_array
      send_large_array(*data, *len, partner, round * 1000 + 1, MPI_COMM_WORLD);
      
      // Questo nodo ha finito, disattivalo
      active = 0;
    }
  }
}


void binary_merge_tree_alt(int **data, size_t *len) {
  if (NULL == *data || NULL == data) {
    printf("[%s] Errore: puntatore a dati da mergiare nullo\n", __func__);
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }
  if (NULL == len) {
    printf("[%s] Errore: puntatore a len nullo\n", __func__);
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }
  if (*len == 0) {
    printf("[%s] Errore: lunghezza array nulla\n", __func__);
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }
  
  int my_rank = -1, nr_nodes = -1;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nr_nodes);
  
  if (2 > nr_nodes) {
    printf("[%s] Numero di nodi troppo basso\n", __func__);
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }

  // Calcola l'upper bound del numero massimo di turni 
  int max_rounds = 0;
  int temp = nr_nodes - 1;
  while (temp > 0) {
    max_rounds++;
    temp >>= 1; // modo efficiente di calcolare la potenza di 2 utile
  }

#ifdef DEBUG
  if (my_rank == 0) {
    printf("[%s,%d] Numero massimo di round: %d per %d nodi\n", __func__, my_rank, max_rounds, nr_nodes);
  }
#endif

  // Array per tracciare quali nodi sono ancora attivi
  int active = 1;
  
  // Ciclo principale
  for (int round = 0; round < max_rounds; round++) {
    int step = 1 << round;
    int group_size = step << 1;
    
    #ifdef DEBUG
    if (0 == my_rank) {
      printf("[%s,%d] round %d\t valore step %d\tvalore group size %d\n",
             __func__, my_rank, round, step, group_size);
    }
    #endif
    
    if (!active) {
      continue;
    }
    
    // Determina il ruolo del nodo in questo round
    int group_id = my_rank / group_size;
    int pos_in_group = my_rank % group_size;
    
    if (pos_in_group < step) {
      // Questo nodo dovrebbe ricevere (se esiste un partner)
      int partner = my_rank + step;
      
      if (partner < nr_nodes) {
        // Il partner esiste, ricevi i dati
        
        // Prima ricevi la dimensione dei dati in arrivo
        size_t incoming_size = 0;
        MPI_Recv(&incoming_size, 1, MPI_UNSIGNED_LONG, partner, round * 1000, 
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
#ifdef DEBUG
        printf("[%s,%d] Round %d: ricevo %zu elementi da rank %d (%.2f GB)\n", __func__, my_rank, round, incoming_size, partner,
               (incoming_size * sizeof(int)) / (1024.0*1024.0*1024.0));
#endif
        
        // Alloca spazio per i dati in arrivo
        int *tmp = malloc(incoming_size * sizeof(int));
        if (NULL == tmp) {
          printf("[%s,%d] Errore allocazione tmp (%zu elementi, %.2f GB)\n",
                 __func__, my_rank, incoming_size,
                 (incoming_size * sizeof(int)) / (1024.0*1024.0*1024.0));
          MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        }
        
        // Ricevi i dati usando recv_large_array
        recv_large_array(tmp, incoming_size, partner, round * 1000 + 1, MPI_COMM_WORLD);
        
        // riallocazione data
        size_t new_size = *len + incoming_size;
        size_t old_size = *len;
        *data = realloc(*data, new_size*sizeof(int));


        if (NULL == *data) {
          printf("[%s,%d] Errore con la realloc di data (%zu elementi, %.2f GB)\n",
                 __func__, my_rank, new_size,
                 (new_size * sizeof(int)) / (1024.0*1024.0*1024.0));
          MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        }
        size_t start_l = new_size-old_size ;
        // sposto data nella parte a destra
        
        memmove(&(*data)[start_l], &(*data)[0], old_size * sizeof(int));
        // Esegui il merge
        merge_opt(*data, new_size, start_l, tmp, incoming_size);
        
        // Sostituisci i dati vecchi con quelli mergiati
        *len = new_size;
        free(tmp);
        
      }
      
    } else {
      // Questo nodo dovrebbe inviare
      int partner = my_rank - step;
      
#ifdef DEBUG
      printf("[%s,%d] Round %d: invio %zu elementi a nodo %d (%.2f GB)\n", __func__, my_rank, round, *len, partner, (*len * sizeof(int)) / (1024.0*1024.0*1024.0));
#endif

      // Invia prima la dimensione
      MPI_Send(len, 1, MPI_UNSIGNED_LONG, partner, round * 1000, MPI_COMM_WORLD);
      
      // Poi invia i dati usando send_large_array
      send_large_array(*data, *len, partner, round * 1000 + 1, MPI_COMM_WORLD);
      
      // Questo nodo ha finito, disattivalo
      active = 0;
    }
  }
}
void test_merge_bint(){
	int my_rank =-1,nr_nodes=-1;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nr_nodes);
  size_t global_size = 25;
  size_t local_size = global_size/nr_nodes;
	int *local_data = malloc(local_size*sizeof(int));


  if(local_data==NULL){
    printf("[%s,%d] Errore creazione data local\n",__func__,my_rank);
    exit(EXIT_FAILURE);
  }

	if(my_rank==0){

		//int data[]={1,5,10,2,7,9,3,4,6,8,11,12,13,14,15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35};
    int *data = malloc(global_size * sizeof(int));
    if( NULL == data){
      printf("%s: errore allocazione data\n",__func__);
      exit(EXIT_FAILURE);
    }

    #pragma omp parallel for 
    for(size_t i=0;i<global_size;i++){
      data[i]=i;
    }
    #ifdef DEBUG
      printArray(data,global_size);
    #endif 

    


		printf("[%s,%d],numero di elementi data totale %ld\n",__func__,my_rank,global_size);

    MPI_Scatter(data, (int)local_size, MPI_INT, local_data, (int)local_size, MPI_INT, 0, MPI_COMM_WORLD);

    free(data);

  }else{

    MPI_Scatter(NULL,(int)local_size, MPI_INT, local_data, (int)local_size, MPI_INT, 0, MPI_COMM_WORLD);
  }



//		printf("[%s,%d],valore locale array:{",__func__,my_rank);
//		for(size_t i=0;i<CHUNK_SIZE;i++)
//			printf("%d,", local_data[i]); 
//		printf("}\n");

  #ifdef DEBUG
  if(my_rank==0)
		printf("[%s],dati locali :\n",__func__);

    printArray(local_data,local_size);
  #endif
  //binary_merge_tree(&local_data,&local_size);
  binary_merge_tree_alt(&local_data,&local_size);


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
    if(global_size != local_size){
      printf("[%s] dimensioni dopo merge errate errore\n",__func__);
      #ifdef DEBUG
        puts("local data:");
        printArray(local_data,local_size);
      #endif
    }
    else{
      puts("dimensioni globali e locali coincidono");

      #ifdef DEBUG
        puts("data:");
        printArray(local_data,local_size);
      #endif
    }
  }

  free(local_data);


}

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
 * La funzione MPI_Alltoall Ã¨ una combinazione efficiente di scatter e gather e permette un efficiente scambio di messaggi 
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
/**
 * dataset : dati contenuti in my_rank ==0 da distribuire 
 * len_datase : dim di dataset 
 * local_data : dati che devono arrivare a un singolo nodo 
 * local_size : len di local data 
 */
/* versione di merge sort che utilizza la libreria mpi per funzionare , distribuisce i dati in chuks a vari nodi mpi che eseguono ordinamento locale 
  * ed eseguono un merge dei chunk ordinati usando la strategia del binary_merge_tree .
  * data dati da ordinare 
  * len la lunghezza dei dati da ordinare 
         */
void merge_sort_mpi(int *data,size_t len){
	int my_rank =-1,nr_nodes=-1;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nr_nodes);

  //Gestione validazione dati locali
	if( (NULL == data) && (my_rank == 0) ){
		printf("[%s] Errore puntatore a data null\n",__func__);
		MPI_Abort(MPI_COMM_WORLD,EXIT_FAILURE);
	}
	if( 0 == len  ){
		if( 0 == my_rank)
			printf("[%s] Errore con la lunghezza indicata in len\n",__func__);
		MPI_Abort(MPI_COMM_WORLD,EXIT_FAILURE);
	}
	if( nr_nodes == 1 ){
		if(len > (1<<18)){
			merge_sort(data,0,len-1);
		}

			return;
	}
	if((nr_nodes & (nr_nodes - 1)) != 0){
		if(my_rank == 0){
			printf("[%s] ERRORE: Il numero di nodi MPI (%d) deve essere una potenza di 2!\n", 
	  __func__, nr_nodes);
			printf("       Valori validi: 2, 4, 8, 16, 32, 64, 128, ...\n");
		}
		MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
	}

	size_t CHUNK_SIZE = len / nr_nodes;
#ifdef DEBUG
	printf("[%s,%d] Lunghezza array originale %ld\tdimensione chunk %zu \n",__func__,my_rank,len,CHUNK_SIZE);
	printf("[%s] Ho passato i controlli preliminari e sto per avviare la distribuzione dei dati\n",__func__);
#endif

	int *local_data = malloc(CHUNK_SIZE * sizeof(int));

	if( NULL == local_data){
		printf("[%s,%d] Errore: problemi allocazione local data.\n",__func__,my_rank);
		MPI_Abort(MPI_COMM_WORLD,EXIT_FAILURE);
	}

	// suddivisione in chunk dei dati

	if( 0 == my_rank ){
		MPI_Scatter(data, CHUNK_SIZE, MPI_INT, local_data, CHUNK_SIZE, MPI_INT, 0, MPI_COMM_WORLD);
	}else{
		MPI_Scatter(NULL, CHUNK_SIZE, MPI_INT, local_data, CHUNK_SIZE, MPI_INT, 0, MPI_COMM_WORLD);
	}
	// ordinamento dati locali 

	size_t local_size = CHUNK_SIZE;

#ifdef DEBUG
  if( 0 == my_rank)
    printf("[%s,%d] Sto lanciando il sort locale sui chunk con local size %ld \n",__func__,my_rank,local_size);
#endif 

	if(0 == local_size){
		if(0 == my_rank)
			printf("[%s] errore DIMENSIONE CHUNK 0 in %d\n",__func__,__LINE__);
		MPI_Abort(MPI_COMM_WORLD,EXIT_FAILURE);
	}

	// Ordinamento locale dei dati
	merge_sort_omp_start(local_data,0,local_size-1);
	//merge_sort(local_data,0,local_size-1);
	MPI_Barrier(MPI_COMM_WORLD);
#ifdef DEBUG
  if( 0==my_rank)
    printf("[%s] lancio il binary merge tree\n",__func__);
#endif 

// merge dati locali
	binary_merge_tree(&local_data,&local_size);

	MPI_Barrier(MPI_COMM_WORLD);

	if( my_rank == 0 ){
#ifdef DEBUG
    printf("[%s] Copio i dati locali in data da local data\n",__func__);
#endif 
		if( local_size != len ){
			printf("[%s,%d] Errore dopo il merge le lunghezze di len e local len non corrispondono\n",__func__,my_rank);
			MPI_Abort(MPI_COMM_WORLD,EXIT_FAILURE);
		}else{
			memcpy(data, local_data, local_size * sizeof(int));
		}
	}
	free(local_data);
}


/* verifica se data è in ordine crescente e restituisce 0 in caso affermativo, se almeno un elemento non è ordinato restituisce 1*/
int isOrdered(int data[],size_t size){
	int ordinato=0;
	for(size_t i=0;i<size-1;i++){
		if(data[i]>data[i+1]){
			ordinato = 1;
			break;
		}
	}
	return ordinato;
}


/** funzione di bechmark dove provo a vedere se funzione la funzione di merge e con quali prestazioni
*/

// Versione di test delle prestazioni dell'algoritmo "merge sort mpi"  dove vengono eseguite le versioni sequenziali e parallerelle
void ben_merge_sort_mpi() { 
  int my_rank = -1, nr_nodes = -1;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nr_nodes);

  size_t SIZE = 1UL << 30;
  int nr_cores = omp_get_num_procs();
  int nr_threads = omp_get_max_threads();
  float execution_time_sequenzial = 0;
  float execution_time_parallel = 0;
  float speed_up = 0;
  int *data_seq = NULL;
  int *data_parall = NULL;
  int status_seq = 0;
  const char *job_id = getenv("SLURM_JOB_ID");
  const char *nnodes_env = getenv("SLURM_NNODES");
  int nnodes_slurm = (nnodes_env) ? atoi(nnodes_env) : -1;
  int nnodes_physical = 0;  // calcolato via MPI_Comm_split_type

  /* Conta i nodi fisici effettivi tramite MPI (portabile anche fuori SLURM) */
  MPI_Comm node_comm;
  MPI_Comm_split_type(MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED, 0, MPI_INFO_NULL, &node_comm);
  int local_rank;
  MPI_Comm_rank(node_comm, &local_rank);
  int is_master = (local_rank == 0) ? 1 : 0;
  MPI_Allreduce(&is_master, &nnodes_physical, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
  MPI_Comm_free(&node_comm);

  if (my_rank == 0) {
    if (job_id) {
      printf("Ambiente HPC rilevato (SLURM job %s)\n", job_id);
      printf("Numero di nodi fisici rilevati da MPI: %d\n", nnodes_physical);
      if (nnodes_slurm > 0)
        printf("Numero di nodi da variabile SLURM_NNODES: %d\n", nnodes_slurm);
    } else {
      printf("Esecuzione locale (nessuna variabile SLURM trovata)\n");
      printf("Numero di nodi fisici (MPI_COMM_TYPE_SHARED): %d\n", nnodes_physical);
    }
  }

  if (my_rank == 0) {
    data_seq = malloc(SIZE * sizeof(int));

    if (data_seq == NULL) {
      printf("[%s,%d] Errore creazione array\n", __func__, my_rank);
      MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    gen_random_numbers(data_seq, SIZE, 0, SIZE);

    struct timeval start, end;
    gettimeofday(&start, NULL);
    merge_sort(data_seq, 0, SIZE - 1);
    gettimeofday(&end, NULL);
    execution_time_sequenzial = tdiff(&start, &end);
    status_seq = isOrdered(data_seq, SIZE);

    free(data_seq);
    data_seq = NULL;
  }

  MPI_Barrier(MPI_COMM_WORLD);

  struct timeval start, end;
  if (my_rank == 0){
    data_parall = malloc(SIZE * sizeof(int));
    if (data_parall == NULL) {
      printf("[%s,%d] Errore creazione data parall\n", __func__, my_rank);
      MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    gen_random_numbers(data_parall, SIZE, 0, 2*SIZE);

    gettimeofday(&start, NULL);
  }

  merge_sort_mpi(data_parall, SIZE);
  //MPI_Barrier(MPI_COMM_WORLD);
  if (my_rank == 0) {
    gettimeofday(&end, NULL);
    execution_time_parallel = tdiff(&start, &end);

    speed_up = execution_time_sequenzial / execution_time_parallel;

    // Efficienza rispetto ai processi MPI
    float eff_mpi = execution_time_sequenzial / (execution_time_parallel * nr_nodes);

    // Efficienza rispetto al totale dei processori logici
    int total_processors = nr_nodes * nr_threads;
    float eff_total = execution_time_sequenzial / (execution_time_parallel * total_processors);

    printf("\n[%s] Statistiche esecuzione merge sort mpi in %s at %s\n", 
           __func__, __DATE__, __TIME__);
    printf("============================================\n");
    printf("dimensione array        = %ld\n", SIZE);
    printf("numero processi MPI     = %d\n", nr_nodes);
    printf("numero nodi fisici      = %d\n", nnodes_physical);
    printf("numero thread OpenMP    = %d\n", nr_threads);
    printf("TOTALE processori logici= %d\n", total_processors);
    printf("numero core fisici      = %d\n", nr_cores);
    printf("============================================\n");
    printf("tempo esecuzione sequenziale  = %0.6f s\n", execution_time_sequenzial);
    printf("tempo esecuzione parallelo    = %0.6f s\n", execution_time_parallel);
    printf("SPEEDUP                       = %0.2f\n", speed_up);
    printf("EFFICIENZA (solo MPI)         = %0.2f (%.1f%%)\n", 
           eff_mpi, eff_mpi * 100);
    printf("EFFICIENZA (MPI×OpenMP)       = %0.2f (%.1f%%)\n", 
           eff_total, eff_total * 100);
    printf("============================================\n");
    printf("correttezza sequenziale = %s\n", 
           ( status_seq == 0 ? "OK" : "Errore"));
    printf("correttezza parallelo   = %s\n", 
           (isOrdered(data_parall, SIZE) == 0 ? "OK" : "Errore"));
    free(data_parall);
  }

}

// Versione di test delle prestazioni dell'algoritmo merge sort mpi dove viene eseguita solo la versione sequenziale.
// Le prestazioni vengono calcolate inserendo manualmente il tempo di esecuzione della versione sequenziale.
void ben_merge_sort_mpi_alt(){
  int my_rank = -1, nr_nodes = -1;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nr_nodes);
  
  size_t SIZE = 1UL << 33 ;
  int nr_cores = omp_get_num_procs();
  int nr_threads = omp_get_max_threads();
  int *data_parall = NULL;
  
  const char *job_id = getenv("SLURM_JOB_ID");
  const char *nnodes_env = getenv("SLURM_NNODES");
  int nnodes_slurm = (nnodes_env) ? atoi(nnodes_env) : -1;
  int nnodes_physical = 0;
  
  const double execution_time_sequential = 2212.470947;
  double execution_time_parallel = 0;
  
  /* Conta i nodi fisici */
  MPI_Comm node_comm;
  MPI_Comm_split_type(MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED, 0, MPI_INFO_NULL, &node_comm);
  int local_rank;
  MPI_Comm_rank(node_comm, &local_rank);
  int is_master = (local_rank == 0) ? 1 : 0;
  MPI_Allreduce(&is_master, &nnodes_physical, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
  MPI_Comm_free(&node_comm);
  
  //stampa statistiche ambiente e allocazione memoria data_parall
  if (my_rank == 0) {
    if (job_id) {
      printf("Ambiente HPC rilevato (SLURM job %s)\nNumero di nodi fisici rilevati da MPI: %d\n", job_id,nnodes_physical);
      if (nnodes_slurm > 0)
        printf("Numero di nodi da variabile SLURM_NNODES: %d\n", nnodes_slurm);
    } else {
      printf("Esecuzione locale\nNumero di nodi fisici (MPI_COMM_TYPE_SHARED): %d\n", nnodes_physical);
    }
    
    // Preparazione dati
    data_parall = malloc(SIZE * sizeof(int));
    if (data_parall == NULL) { 
      printf("[%s,%d] Errore allocazione memoria\n", __func__, my_rank);
      MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    gen_random_numbers(data_parall, SIZE, 0, 2*SIZE);
  }
  
  MPI_Barrier(MPI_COMM_WORLD);
  
  // Rank 0 inizia a misurare
  double start_time = 0;
  if (my_rank == 0) {
    start_time = MPI_Wtime();
  }
  
  // Algoritmo parallelo
  merge_sort_mpi(data_parall, SIZE);
  
  // Rank 0 termina la misurazione
  if (my_rank == 0) {
    execution_time_parallel = MPI_Wtime() - start_time;
    double speed_up = execution_time_sequential / execution_time_parallel;
    double eff_mpi = speed_up / nr_nodes;
    int total_processors = nr_nodes * nr_threads;
    double eff_total = speed_up / total_processors;

    printf("\n[%s] Statistiche Merge Sort MPI\n", __func__);
    printf("Compilato: %s at %s\n", __DATE__, __TIME__);
    printf("============================================\n");
    printf("Configurazione:\n");
    printf("  dimensione array         = %ld elementi (2^33)\n", SIZE);
    printf("  memoria richiesta        = %.2f GB\n", (SIZE * sizeof(int)) / 1e9);
    printf("  numero processi MPI      = %d\n", nr_nodes);
    printf("  numero nodi fisici       = %d\n", nnodes_physical);
    printf("  thread OpenMP per proc   = %d\n", nr_threads);
    printf("  TOTALE processori logici = %d\n", total_processors);
    printf("  core fisici disponibili  = %d\n", nr_cores);
    printf("============================================\n");
    printf("Prestazioni:\n");
    printf("  tempo sequenziale        = %.3f s (%.2f min)\n", 
           execution_time_sequential, execution_time_sequential / 60.0);
    printf("  tempo parallelo          = %.3f s (%.2f min)\n", 
           execution_time_parallel, execution_time_parallel / 60.0);
    printf("  SPEEDUP ASSOLUTO         = %.2fx\n", speed_up);
    printf("  Efficienza MPI           = %.2f%% (%.2fx su %d proc)\n", 
           eff_mpi * 100, eff_mpi, nr_nodes);
    printf("  Efficienza totale        = %.2f%% (%.2fx su %d proc)\n", 
           eff_total * 100, eff_total, total_processors);
    printf("  Throughput               = %.2f M elem/s\n", 
           SIZE / (execution_time_parallel * 1e6));
    printf("============================================\n");
    printf("Verifica:\n");
    printf("  ordinamento corretto     = %s\n", 
           (isOrdered(data_parall, SIZE) == 0 ? "V OK" : "? ERRORE"));
    printf("  precisione timer MPI     = %.9f s\n", MPI_Wtick());
    printf("============================================\n");
    
    free(data_parall);
  }
}

/**
 * Merge ottimizzato con workspace riusabile
 * 
 * PREREQUISITO:
 * - data[0..len_left-1] contiene elementi left ORDINATI
 * - data[len_left..len_left+len_right-1] contiene elementi right ORDINATI
 * 
 * @param data: array con left e right contigui
 * @param len_left: numero elementi left
 * @param len_right: numero elementi right
 * @param workspace: buffer temporaneo >= len_left+len_right
 */
void merge_with_workspace(int *data, size_t len_left, size_t len_right, int *workspace) {
    size_t i = 0;              // Indice per left
    size_t j = len_left;       // Indice per right (inizia dopo left)
    size_t k = 0;              // Indice per workspace
    size_t total = len_left + len_right;
    
    // Merge dei due sottoarray ordinati nel workspace
    while (i < len_left && j < total) {
        if (data[i] <= data[j]) {
            workspace[k++] = data[i++];
        } else {
            workspace[k++] = data[j++];
        }
    }
    
    // Copia elementi rimanenti da left
    while (i < len_left) {
        workspace[k++] = data[i++];
    }
    
    // Copia elementi rimanenti da right
    while (j < total) {
        workspace[k++] = data[j++];
    }
    
    // Copia risultato finale in data
    memcpy(data, workspace, total * sizeof(int));
}

void binary_merge_tree_optimized(int **data, size_t *len) {
    // Validazione input
    if (NULL == *data || NULL == data) {
        fprintf(stderr, "[%s] Errore: puntatore a dati nullo\n", __func__);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    if (NULL == len || *len == 0) {
        fprintf(stderr, "[%s] Errore: lunghezza invalida\n", __func__);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    
    int my_rank, nr_nodes;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nr_nodes);
    
    if (nr_nodes < 2) {
        return;
    }
    
    // Calcola numero massimo di round
    int max_rounds = 0;
    int temp = nr_nodes - 1;
    while (temp > 0) {
        max_rounds++;
        temp >>= 1;
    }
    
    #ifdef DEBUG
    if (my_rank == 0) {
        printf("[%s] Max rounds: %d per %d nodi\n", __func__, max_rounds, nr_nodes);
    }
    #endif
    
    // ===== WORKSPACE RIUSABILE =====
    // Allocato UNA VOLTA, cresciuto se necessario, riusato per tutti i round
    int *workspace = NULL;
    size_t workspace_capacity = 0;
    
    // Ciclo principale
    for (int round = 0; round < max_rounds; round++) {
        int step = 1 << round;
        int group_size = step << 1;
        int pos_in_group = my_rank % group_size;
        
        #ifdef DEBUG
        if (my_rank == 0) {
            printf("[%s] Round %d: step=%d, group_size=%d\n", 
                   __func__, round, step, group_size);
        }
        #endif
        
        if (pos_in_group < step) {
            // ===== RECEIVER =====
            int partner = my_rank + step;
            
            if (partner >= nr_nodes) {
                continue; // Nessun partner disponibile
            }
            
            // Ricevi dimensione in arrivo
            size_t incoming_size;
            MPI_Recv(&incoming_size, 1, MPI_UNSIGNED_LONG, partner, 
                     round * 1000, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            #ifdef DEBUG
            printf("[%s,%d] Round %d: ricevo %zu elementi da rank %d (%.2f GB)\n", 
                   __func__, my_rank, round, incoming_size, partner,
                   (incoming_size * sizeof(int)) / (1024.0*1024.0*1024.0));
            #endif
            
            size_t old_size = *len;
            size_t new_size = old_size + incoming_size;
            
            // ===== STEP 1: Espandi array =====
            int *new_data = realloc(*data, new_size * sizeof(int));
            if (NULL == new_data) {
                fprintf(stderr, "[%s,%d] Errore realloc: %zu elementi (%.2f GB)\n",
                        __func__, my_rank, new_size,
                        (new_size * sizeof(int)) / (1024.0*1024.0*1024.0));
                free(*data);
                free(workspace);
                MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
            }
            *data = new_data;
            
            recv_large_array(*data + old_size, incoming_size, partner, 
                           round * 1000 + 1, MPI_COMM_WORLD);
            
            // ===== STEP 3: Alloca/espandi workspace se necessario =====
            if (workspace_capacity < new_size) {
                int *new_workspace = realloc(workspace, new_size * sizeof(int));
                if (NULL == new_workspace) {
                    fprintf(stderr, "[%s,%d] Errore workspace realloc: %zu elementi\n",
                            __func__, my_rank, new_size);
                    free(workspace);
                    free(*data);
                    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
                }
                workspace = new_workspace;
                workspace_capacity = new_size;
            }
            
            
            merge_with_workspace(*data, old_size, incoming_size, workspace);
            
            // Aggiorna lunghezza
            *len = new_size;
            
        } else {
            // ===== SENDER =====
            int partner = my_rank - step;
            
            #ifdef DEBUG
            printf("[%s,%d] Round %d: invio %zu elementi a rank %d (%.2f GB)\n",
                   __func__, my_rank, round, *len, partner, 
                   (*len * sizeof(int)) / (1024.0*1024.0*1024.0));
            #endif
            
            // Invia dimensione
            MPI_Send(len, 1, MPI_UNSIGNED_LONG, partner, 
                     round * 1000, MPI_COMM_WORLD);
            
            // Invia dati
            send_large_array(*data, *len, partner, 
                           round * 1000 + 1, MPI_COMM_WORLD);
            
            // Questo nodo ha finito - cleanup e esci
            if (workspace != NULL) {
                free(workspace);
                workspace = NULL;  // Evita double free
            }
            return;  // Esci dalla funzione, non solo dal loop
        }
    }
    
    // Cleanup finale (solo receiver che completano tutti i round)
    if (workspace != NULL) {
        free(workspace);
        workspace = NULL;
    }
}
void quick_sort_mpi(int *data,size_t len){
	int my_rank =-1,nr_nodes=-1;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nr_nodes);
  if( nr_nodes == 1 ){
    if(len > (1<<18)){
      quick_sort_omp_start(data,0,len-1,MEDIAN_ACTIVATION);
      return; }else{
      median_quick_sort(data,0,len-1);
      return;
    }

  }
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
if((nr_nodes & (nr_nodes - 1)) != 0){
        if(my_rank == 0){
            printf("[%s] ERRORE: Il numero di nodi MPI (%d) deve essere una potenza di 2!\n", 
                   __func__, nr_nodes);
            printf("       Valori validi: 2, 4, 8, 16, 32, 64, 128, ...\n");
        }
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
	
	size_t CHUNK_SIZE = len / nr_nodes;
	#ifdef DEBUG
		printf("[%s,%d] Lunghezza array originale %ld\tdimensione chunk %zu \n",__func__,my_rank,len,CHUNK_SIZE);
	#endif
  if( len <= CHUNK_SIZE ){
    quick_sort_omp_start(data,0,len-1,MEDIAN_ACTIVATION);
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
	quick_sort_omp_start(local_data,0,local_size-1,MEDIAN_ACTIVATION);

  // merge
  MPI_Barrier(MPI_COMM_WORLD);

  #ifdef DEBUG
  printf("[%s] lancio il binary merge tree\n",__func__);
  #endif 
  //binary_merge_tree(&local_data,&local_size);
  binary_merge_tree_alt(&local_data,&local_size);
  //binary_merge_tree_optimized(&local_data,&local_size);
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



void quick_sort_mpi_alt(int *data, size_t len) {

    int my_rank, nr_nodes;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nr_nodes);

    // Controlli base
    if (data == NULL && my_rank == 0) {
        fprintf(stderr, "[%s] Errore: puntatore data NULL\n", __func__);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    if (len == 0) {
        fprintf(stderr, "[%s] Errore: len == 0\n", __func__);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    // Caso singolo nodo  fallback a OpenMP
    if (nr_nodes == 1) {
        if (len > (1 << 18))
            quick_sort_omp_start(data, 0, len - 1, MEDIAN_ACTIVATION);
        else
            median_quick_sort(data, 0, len - 1);
        return;
    }

    // Potenza di 2
    if ((nr_nodes & (nr_nodes - 1)) != 0) {
        if (my_rank == 0)
            fprintf(stderr, "[%s] ERRORE: nr_nodes deve essere potenza di 2\n", __func__);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    // Gestione divisione
    size_t CHUNK_SIZE = len / nr_nodes;
    size_t remainder  = len % nr_nodes;

    // Allocazione locale allineata
    int *local_data = aligned_alloc(64, CHUNK_SIZE * sizeof(int));
    if (!local_data) {
        fprintf(stderr, "[%s,%d] allocazione fallita\n", __func__, my_rank);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    // Distribuzione dati
    MPI_Scatter(data, (int)CHUNK_SIZE, MPI_INT,
                local_data, (int)CHUNK_SIZE, MPI_INT,
                0, MPI_COMM_WORLD);

    // Sort locale parallelo
    quick_sort_omp_start(local_data, 0, CHUNK_SIZE - 1, MEDIAN_ACTIVATION);

    // Merge tree distribuito
    binary_merge_tree(&local_data, &CHUNK_SIZE);

    // Rank 0 raccoglie
    if (my_rank == 0)
        memcpy(data, local_data, len * sizeof(int));

    free(local_data);
}


void ben_quick_sort_mpi(){
  int my_rank = -1, nr_nodes = -1;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nr_nodes);
    
    size_t SIZE = 1<<30;
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
            printf("[%s,%d] Errore creazione array data seq di %ld elementi\n",
                   __func__, my_rank, SIZE);
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        }
        if (data_parall == NULL) {
            printf("[%s,%d] Errore creazione array data parall di %ld elementi\n",
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
        median_quick_sort(data_seq, 0,SIZE - 1);
        gettimeofday(&end, NULL);
        execution_time_sequenzial = tdiff(&start, &end);
        
#ifdef DEBUG
        printf("[%s] Ordinamento sequenziale ok\n", __func__);
#endif
    }
    
    // Sincronizza tutti prima del timing parallelo
    MPI_Barrier(MPI_COMM_WORLD);
    
    struct timeval start, end;
    if (my_rank == 0) {
        gettimeofday(&start, NULL);
    }
    
    // *** TUTTI i processi chiamano merge_sort_mpi ***
    quick_sort_mpi(data_parall, SIZE);
    
    if (my_rank == 0) {
        gettimeofday(&end, NULL);
        execution_time_parallel = tdiff(&start, &end);
        
        speed_up = execution_time_sequenzial / execution_time_parallel;
        eff = execution_time_sequenzial / (execution_time_parallel * nr_nodes);
        
        printf("[%s] Statistiche esecuzione merge sort mpi in %s at %s\n", 
               __func__, __DATE__, __TIME__);
        printf("dimensione array= %ld\n", SIZE);
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
        
        
        free(data_seq);
        free(data_parall);
    }
}

void ben_quick_sort_mpi_alt(){
  int my_rank = -1, nr_nodes = -1;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nr_nodes);
  
  size_t SIZE = 1UL << 25 ;
  int nr_cores = omp_get_num_procs();
  int nr_threads = omp_get_max_threads();
  int *data_parall = NULL;
  
  const char *job_id = getenv("SLURM_JOB_ID");
  const char *nnodes_env = getenv("SLURM_NNODES");
  int nnodes_slurm = (nnodes_env) ? atoi(nnodes_env) : -1;
  int nnodes_physical = 0;
  
  const double execution_time_sequential = 2212.470947;
  double execution_time_parallel = 0;
  
  /* Conta i nodi fisici */
  MPI_Comm node_comm;
  MPI_Comm_split_type(MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED, 0, MPI_INFO_NULL, &node_comm);
  int local_rank;
  MPI_Comm_rank(node_comm, &local_rank);
  int is_master = (local_rank == 0) ? 1 : 0;
  MPI_Allreduce(&is_master, &nnodes_physical, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
  MPI_Comm_free(&node_comm);
  
  //stampa statistiche ambiente e allocazione memoria data_parall
  if (my_rank == 0) {
    if (job_id) {
      printf("Ambiente HPC rilevato (SLURM job %s)\nNumero di nodi fisici rilevati da MPI: %d\n", job_id,nnodes_physical);
      if (nnodes_slurm > 0)
        printf("Numero di nodi da variabile SLURM_NNODES: %d\n", nnodes_slurm);
    } else {
      printf("Esecuzione locale\nNumero di nodi fisici (MPI_COMM_TYPE_SHARED): %d\n", nnodes_physical);
    }
    
    // Preparazione dati
    data_parall = malloc(SIZE * sizeof(int));
    if (data_parall == NULL) { 
      printf("[%s,%d] Errore allocazione memoria\n", __func__, my_rank);
      MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    gen_random_numbers(data_parall, SIZE, 0, 2*SIZE);
  }
  
  MPI_Barrier(MPI_COMM_WORLD);
  
  // Rank 0 inizia a misurare
  double start_time = 0;
  if (my_rank == 0) {
    start_time = MPI_Wtime();
  }
  
  // Algoritmo parallelo
  quick_sort_mpi(data_parall, SIZE);
  
  // Rank 0 termina la misurazione
  if (my_rank == 0) {
    execution_time_parallel = MPI_Wtime() - start_time;
    double speed_up = execution_time_sequential / execution_time_parallel;
    double eff_mpi = speed_up / nr_nodes;
    int total_processors = nr_nodes * nr_threads;
    double eff_total = speed_up / total_processors;

    printf("\n[%s] Statistiche Merge Sort MPI\n", __func__);
    printf("Compilato: %s at %s\n", __DATE__, __TIME__);
    printf("============================================\n");
    printf("Configurazione:\n");
    printf("  dimensione array         = %ld elementi (2^28)\n", SIZE);
    printf("  memoria richiesta        = %.2f GB\n", (SIZE * sizeof(int)) / 1e9);
    printf("  numero processi MPI      = %d\n", nr_nodes);
    printf("  numero nodi fisici       = %d\n", nnodes_physical);
    printf("  thread OpenMP per proc   = %d\n", nr_threads);
    printf("  TOTALE processori logici = %d\n", total_processors);
    printf("  core fisici disponibili  = %d\n", nr_cores);
    printf("============================================\n");
    printf("Prestazioni:\n");
    printf("  tempo sequenziale        = %.3f s (%.2f min)\n", 
           execution_time_sequential, execution_time_sequential / 60.0);
    printf("  tempo parallelo          = %.3f s (%.2f min)\n", 
           execution_time_parallel, execution_time_parallel / 60.0);
    printf("  SPEEDUP ASSOLUTO         = %.2fx\n", speed_up);
    printf("  Efficienza MPI           = %.2f%% (%.2fx su %d proc)\n", 
           eff_mpi * 100, eff_mpi, nr_nodes);
    printf("  Efficienza totale        = %.2f%% (%.2fx su %d proc)\n", 
           eff_total * 100, eff_total, total_processors);
    printf("  Throughput               = %.2f M elem/s\n", 
           SIZE / (execution_time_parallel * 1e6));
    printf("============================================\n");
    printf("Verifica:\n");
    printf("  ordinamento corretto     = %s\n", 
           (isOrdered(data_parall, SIZE) == 0 ? "V OK" : "? ERRORE"));
    printf("  precisione timer MPI     = %.9f s\n", MPI_Wtick());
    printf("============================================\n");
    
    free(data_parall);
  }

}

void find_error_scatter(){
  size_t OSIZE = 1UL << 28;
	int my_rank =-1,nr_nodes=-1;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nr_nodes);

	size_t CHUNK_SIZE = OSIZE / nr_nodes;
	int *local_data = malloc(CHUNK_SIZE*sizeof(int));

	if(my_rank==0){

		int *data = malloc(OSIZE * sizeof(int));
    if( data == NULL ){
      printf("[%s] errore creazione data\n",__func__);
      MPI_Abort(MPI_COMM_WORLD,EXIT_FAILURE);
    }

    gen_random_numbers(data, OSIZE, 0, OSIZE);


		printf("[%s,%d],numero di elementi data totale %ld : Caso distribuzione corretta\n",__func__,my_rank,CHUNK_SIZE*nr_nodes);

    MPI_Scatter(data, (int)CHUNK_SIZE, MPI_INT, local_data, (int)CHUNK_SIZE, MPI_INT, 0, MPI_COMM_WORLD);

    free(data);

  }else{

    MPI_Scatter(NULL, CHUNK_SIZE, MPI_INT, local_data, CHUNK_SIZE, MPI_INT, 0, MPI_COMM_WORLD);
  }

   if( 0 == my_rank){
   	printf("Test lancio binary merge con dati giganteschi\n");
   } 

   binary_merge_tree(&local_data,&CHUNK_SIZE);
    if( 0 == my_rank){
    printf("[%s] Dimensione originale di test %zu\n",__func__,OSIZE);
	    
	    if( CHUNK_SIZE == OSIZE)
		    printf("Dimensioni coerenti dopo la merge\n");
	    else
		    printf("Problemi con le dimenisoni CHUNCK_SIZE = %ld\t dimesione originale = %ld\n",CHUNK_SIZE,OSIZE);

    //printf("dimensione originale %ld\tdimensione chunck %ld\n",OSIZE,CHUNK_SIZE);
    }




//  printf("[%s,%d],dati distribuiti:{",__func__,my_rank);
  free(local_data);
}

int main(int argc,char *argv[]){
  MPI_Init(&argc, &argv);
  //test_init();
  //test_merge();
  //test_merge_bint();
  //find_error_scatter();
  ben_merge_sort_mpi();
  //ben_merge_sort_mpi_alt();
  //ben_quick_sort_mpi();
  //ben_quick_sort_mpi_alt();
  //find_error_scatter();
  MPI_Finalize();
  return 0;
}
