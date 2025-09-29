#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(){
  size_t array_size = 5;
  printf("dimensione array %ld\n",array_size);
  for(int i=1;i<=10;i++){
    int ideal_chunk_size = array_size/i;
    double min_chunck_size = (double)array_size/(double)i;

    printf("[%s] numero processi %d\t dimensione ideale(upper bound) %d\tlower bound %f\n",__func__,i,ideal_chunk_size,min_chunck_size);
  }

}
