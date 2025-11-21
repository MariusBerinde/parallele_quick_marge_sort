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

void printArrayLong(long *data,size_t LEN){ 
  printf("[%s] stampa array:{\t",__func__);
  size_t i;
  for( i=0;i<LEN-1;i++)
    printf("%ld,",data[i]);
  printf("%ld }\n",data[i]);
}

void gen_random_numbers(int *array, int len, int min, int max){
 const unsigned int SEED = 42; 
  srand(SEED);
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
void old_content(){
  size_t SIZE = 10;
  int *d = malloc(SIZE*sizeof(int));
  if( NULL == d ){
    printf("[%s] errore allocazione memoria con d\n",__func__);
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
void merge_opt(long *data,size_t len,size_t old_len,long *right,size_t len_rig){
  size_t i=0,j=old_len,k=0;
  while(j<len && k<len_rig){
    data[i++] = (data[j]<=right[k])?(data[j++]):(right[k++]);
  }

  if(j<len){
    memcpy(&data[i], &data[j], (len - j) * sizeof(long));
    i += (len-j);
  }
  if(k<len_rig){
    memcpy(&data[i], &right[k], (len_rig - k) * sizeof(long));
    i += (len_rig - k);
  }

}
void test_merge_opt(){
  size_t size_l = 4, size_r = 2;
  long *left = malloc(size_l * sizeof(long));
  if( NULL == left){
    printf("[%s] errore con allocazione left\n",__func__);
    exit(EXIT_FAILURE);
  }

  long *right = malloc(size_r * sizeof(long));

  if( NULL == right){
    printf("[%s] errore con allocazione right\n",__func__);
    exit(EXIT_FAILURE);
  }
  left[0] = 0, left[1] = 4, left[2] = 11,left[3] = 8;
  right[0] = 3, right[1] = 5;//;
  puts("[test_merge_opt] print left");
  printArrayLong(left,size_l);
  puts("[test_merge_opt] print right");
  printArrayLong(right,size_r);
  size_t old_size_l = size_l;
  size_l = old_size_l + size_r;

  left = realloc(left, size_l * sizeof(long));
  if (NULL == left){
    printf("[%s] errore con la realloc\n",__func__);
    exit(EXIT_FAILURE);
  }
  size_t start_l = size_l - old_size_l;
  memmove(&left[start_l],&left[0],old_size_l*sizeof(long));
  puts("dati dopo la move");
  printArrayLong(left,size_l);
  puts("unione dati");
  merge_opt(left,size_l,start_l,right,size_r);
  puts(" dati dopo unione:");
  printArrayLong(left,size_l);
  
}
void test_merge_eq_size(){
size_t size_l = 3, size_r = 3;
  long *left = malloc(size_l * sizeof(long));
  if( NULL == left){
    printf("[%s] errore con allocazione left\n",__func__);
    exit(EXIT_FAILURE);
  }

  long *right = malloc(size_r * sizeof(long));

  if( NULL == right){
    printf("[%s] errore con allocazione right\n",__func__);
    exit(EXIT_FAILURE);
  }
  right[0] = 0, right[1] = 4, right[2] = 6;
  left[0] = 7, left[1] = 11 ,left[2] = 15;//;
  puts("[test_merge_opt] print left");
  printArrayLong(left,size_l);
  puts("[test_merge_opt] print right");
  printArrayLong(right,size_r);
  size_t old_size_l = size_l;
  size_l = old_size_l + size_r;

  left = realloc(left, size_l * sizeof(long));
  if (NULL == left){
    printf("[%s] errore con la realloc\n",__func__);
    exit(EXIT_FAILURE);
  }
  size_t start_l = size_l - old_size_l;
  memmove(&left[start_l],&left[0],old_size_l*sizeof(long));
  puts("dati dopo la move");
  printArrayLong(left,size_l);
  puts("unione dati");
  merge_opt(left,size_l,start_l,right,size_r);
  puts(" dati dopo unione:");
  printArrayLong(left,size_l);
}

int main(){
  //old_content();
  test_merge_opt();
  //test_merge_eq_size();
}
