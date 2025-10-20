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

void gen_random_numbers(int *array, int len, int min, int max){
 const unsigned int SEED = 42; 
  srand(SEED);
  srand(time(0));
    for (int i = 0; i < len; i++)
        array[i] = rand() % (max - min + 1) + min;
}
void test_ciclo(int n){
  size_t MAX = 100;
  while(n--){
    int a = rand() & MAX; 
    printf("%d ",a);
  }
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

  int *a = malloc(SIZE*sizeof(int));
  int *b = malloc(SIZE*sizeof(int));

  gen_random_numbers(a,SIZE,0,SIZE);
  gen_random_numbers(b,SIZE,0,SIZE);

  puts("Stampo a");
  printArray(a,SIZE);


  puts("Stampo b");
  printArray(b,SIZE);

  free(a);
  free(b);
 test_ciclo(SIZE); 
}
