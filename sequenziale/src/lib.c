#include <stdio.h>
#include <stdlib.h>

/**
 * funzione usata per scambiare gli elementi in posizione first e second nell'array data.
 */
void swap(int *data,size_t first,size_t second){
			int tmp = data[first];
			data[first]=data[second];
			data[second]=tmp;
}


/**
 * funzione che calcola il pivot 
 */
unsigned int partition(int* data,unsigned int start,unsigned int end){
  int pivot = data[end];
  unsigned int index_pivot=start-1;
  for(size_t i=start;i<end;i++){
    if(data[i]<pivot){
      index_pivot++;
      swap(data,i,index_pivot);
    }
  }
  swap(data,index_pivot,end);
  return index_pivot+1;
}

unsigned int part2(int* data,int start,int end){
  int pivot = data[start];
  unsigned int i = start;
  unsigned int j = end;
  while (i<j) {
    while (data[i] < pivot && i < end-1) {
      i++;
    }
    while (data[j]> pivot && j >= start+1) {
      j--;
    }
    if(i<j)
      swap(data,i,j);
  }
  swap(data,start,j);
  return j;

}
/**
 * quick sort ricorsivo
 * begin è l'inizio dell'intervallo
 * end è la fine dell'intervallo
 *
 * */
void quick_sort_ric(int *data,unsigned int begin,unsigned int end){
  /*
  if(end-begin==1){

    printf("[%s] begin=%u\t end=%u\n",__func__,begin,end);
    if(data[begin]>data[end])
      swap(data,begin,end);
  }
  else
  */
  if(begin<end){

    printf("[%s] begin=%u\t end=%u\n",__func__,begin,end);
  unsigned int p = partition(data,begin,end);

    printf("[%s] pivot index %u \n",__func__,p);

    quick_sort_ric(data,begin,p-1);
    quick_sort_ric(data,p+1,end-1);
  }

}




/** implementazione di quick sort ricorsivo**/

void quick_sort(int *data,size_t len){
    printf("[%s] begin=%d\t end %ld\n",__func__,0,len);
	quick_sort_ric(data,0,len);

}

void merge(int data[],int low,int mid,int high){
  int i,j,k;
  int n1 = mid-low+1;
  int n2 = high-mid;
  int tmpLeft[n1],tmpRight[n2];

  for (i=0;i<n1;i++) {
    tmpLeft[i] = data[low+i];
  }
  for(j=0;j<n2;j++){
    tmpRight[j] = data[mid+1+j];
  }

  //merge tmpLeft and tmpRight
  i=0; j=0; k=low;
  while(i<n1 &&j<n2){
    if(tmpLeft[i] <= tmpRight[j]){
      data[k]=tmpLeft[i];
      i++;
    }else{

      data[k]=tmpRight[j];
      j++;
    }
    k++;

  }
  while(i<n1){
    data[k++]= tmpLeft[i++];
    //i++;
    //k++;
  }

  while(j<n2){
    data[k++]= tmpRight[j++];
    //j++;
    //k++;
  }

}

void merge2(int arr[], int left, int mid, int right) {
    int i, j, k;
    int n1 = mid - left + 1;
    int n2 = right - mid;

    // Create temporary arrays
    int leftArr[n1], rightArr[n2];

    // Copy data to temporary arrays
    for (i = 0; i < n1; i++)
        leftArr[i] = arr[left + i];
    for (j = 0; j < n2; j++)
        rightArr[j] = arr[mid + 1 + j];

    // Merge the temporary arrays back into arr[left..right]
    i = 0;
    j = 0;
    k = left;
    while (i < n1 && j < n2) {
        if (leftArr[i] <= rightArr[j]) {
            arr[k] = leftArr[i];
            i++;
        }
        else {
            arr[k] = rightArr[j];
            j++;
        }
        k++;
    }

    // Copy the remaining elements of leftArr[], if any
    while (i < n1) {
        arr[k] = leftArr[i];
        i++;
        k++;
    }

    // Copy the remaining elements of rightArr[], if any
    while (j < n2) {
        arr[k] = rightArr[j];
        j++;
        k++;
    }
}
/**
  * main corp of merge sort 
*/
void merge_sort(int* data,int len,int left,int right){
  if(left<right){
    int center = (left+right)/2;
    printf("%s, center = %d\n",__func__,center);
    merge_sort(data,len,left,center);
    merge_sort(data,len,center+1,right);
    merge(data,left,center,right);
    //merge2(data,left,center,right);
    
  }
}
void test_quick_sort_local(){
	int MAX=6;
	int v[]={5,4,3,2,1,11};
	printf("[%s]: Vettore prima dell'ordinamento :",__func__);
  
  
	for(size_t i=0;i<MAX;i++){
		printf("[%s]: v[%ld] = %d\t :",__func__,i,v[i]);
	}
  puts("\n");
     
	quick_sort(v,MAX-1);
	printf("[%s]: Vettore dopo oridinamento \n",__func__);
	for(size_t i=0;i<MAX;i++){
		printf("[%s]: v[%ld] = %d\n :",__func__,i,v[i]);
	}

}

void test_merge_sort(){
  int v[]={38,27,43,3,9,82,10};
  int MAX=7;

	printf("[%s]: Vettore prima dell'ordinamento :",__func__);
  
  
	for(size_t i=0;i<MAX;i++){
		printf("[%s]: v[%ld] = %d\t :",__func__,i,v[i]);
	}
  puts("\n");
     
	merge_sort(v,MAX,0,MAX-1);
	printf("[%s]: Vettore dopo oridinamento \n",__func__);
	for(size_t i=0;i<MAX;i++){
		printf("[%s]: v[%ld] = %d\n :",__func__,i,v[i]);
	}
}



int main(){
	test_quick_sort_local();
  test_merge_sort();

}
