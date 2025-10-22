/**
* file in cui provo a creare un tipo per la comunicazione con mpi e creare un grafo usando quel tipo
*/
#include <mpi.h>
#include <stddef.h>  
#include <stdio.h>
#include <stdlib.h> 
//#include <openmpi.h>

typedef struct{
  int rank;
  int max;
  int min;
}Nodo;


void init_nodo(MPI_Datatype *MPI_NODO){
  int block_lengths[3]={1,1,1}; // se avessi array o altre strutture dovrei mettere la loro lunghezza
  MPI_Datatype types[3]={MPI_INT,MPI_INT,MPI_INT}; // il corrispettivo dei tipi primitvi che creano il nuovo tipo 
  MPI_Aint displacements[3] ;
  displacements[0] = offsetof(Nodo,rank);
  displacements[1] = offsetof(Nodo,max);
  displacements[2] = offsetof(Nodo,min);

  MPI_Type_create_struct(3,block_lengths,displacements,types,MPI_NODO);
  MPI_Type_commit(MPI_NODO);

}


void test_struct(){
  Nodo a;
  a.rank = 0;
  a.max = 10;
  a.min = 0;

  printf("[%s] Ho creato un nodo {Rank=%d,(%d,%d)}\n",__func__,a.rank,a.min,a.max);
}

void test_send_node(){
	int my_rank =-1,nr_nodes=-1;

  MPI_Datatype MPI_NODO;
  init_nodo(&MPI_NODO);

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nr_nodes);
  if(2 != nr_nodes){
    printf("[%s] Errore per questo test devi usare 2 nodi \n",__func__);
    MPI_Abort(MPI_COMM_WORLD,EXIT_FAILURE);
  }

  if(0 == my_rank){
    Nodo a;
    a.rank = 0;
    a.max = 5;
    a.min = 3;

    printf("[%s,%d] invio nodo ....\n",__func__,my_rank);

    #ifdef DEBUG
      
    printf("[%s,%d] caratteristiche nodo {rank = %d\t(min=%d,max=%d) } \n",__func__,my_rank,a.rank,a.min,a.max);
    #endif 

     MPI_Send(&a,1,MPI_NODO,1,0,MPI_COMM_WORLD);
    
  }
  if(1 == my_rank){
    Nodo tmp;
    MPI_Recv(&tmp,1,MPI_NODO,0,0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    printf("[%s,%d] nodo ricevuto valori :\n{\n\trank=%d\n\tmin=%d\n\tmax=%d\n}\n \n",__func__,my_rank,tmp.rank,tmp.min,tmp.max);

  }

  MPI_Type_free(&MPI_NODO);

}


void test_init(){
	int my_rank =-1,nr_nodes=-1;

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nr_nodes);
	printf("[%s,%d] Ciao sono %d e ci sono %d processi attivi\n",__func__,my_rank,my_rank,nr_nodes);
}

int main(int argc,char *argv[]){
  MPI_Init(&argc, &argv);


  //test_struct();
  test_init();
  //test_send_node();
  MPI_Finalize();
}
