#include "../src/lib.c"
/** rapido test per verificare se quick sort funziona*/

void test_quick_sort(){
	int MAX=10;
	int v[]={10,9,8,7,6,5,4,3,2,1};
	printf("[%s]: Vettore prima dell'ordinamento :",__func__);
	for(size_t i=0;i<MAX;i++){
		printf("[%s]: v[%ld] = %d\t :",__func__,i,v[i]);
	}
	puts("\n");
	printf("[%s]: Vettore dopo oridinamento :",__func__);
	quick_sort(v,MAX);
	for(size_t i=0;i<MAX;i++){
		printf("[%s]: v[%ld] = %d\t :",__func__,i,v[i]);
	}
	puts("\n");

}
int main(){
	test_quick_sort();
  return  0;
}
