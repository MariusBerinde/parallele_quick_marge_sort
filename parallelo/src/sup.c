#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <math.h>

void printArray(int *data,size_t LEN){
  printf("[%s] stampa array:{\t",__func__);
  size_t i;
  for( i=0;i<LEN-1;i++)
    printf("%d,",data[i]);
  printf("%d }\n",data[i]);

}

int main(){
  size_t SIZE = 10;
  int *d = malloc(SIZE*sizeof(int));
  if( NULL == d ){
    printf("[%d] errore allocazione memoria con d\n",__func__);
    exit(EXIT_FAILURE);
  }
  for(size_t i=0;i<SIZE;d[i]=i,i++);
  // uso standard mi aspetto tutto l'array
  printArray(d,SIZE);
  //provo solo da 4 a finale
  printArray(&(d[4]),SIZE-4);


  free(d);

}
