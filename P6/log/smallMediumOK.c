#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdint.h>

#define MAX_LINES 1000000
#define MAX_LINE_LENGTH 200

uint64_t get_current_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t result = tv.tv_sec*(uint64_t)1000000 + tv.tv_usec;
    return result;
} 

typedef struct keyValuePair
{
  int key;
  char *value;
} keyValuePair;

/* create thread argument struct for thr_func() */
typedef struct _thread_data_t {
  keyValuePair **arr;
  int l;
  int r;
} thread_data_t;

// Merges two subarrays of arr[].
// First subarray is arr[l..m]
// Second subarray is arr[m+1..r]
void merge(keyValuePair **arr, int l, int m, int r) // l = 1, m = 2, r = 2
{
  int i, j, k;
  int n1 = m - l + 1; 
  int n2 = r - m; 

  /* create temp arrays */
  keyValuePair *L[n1], *R[n2];

  /* Copy data to temp arrays L[] and R[] */
  for (i = 0; i < n1; i++)  
  {
    L[i] = arr[l + i];
  }
  for (j = 0; j < n2; j++)
  {
    R[j] = arr[m + 1 + j];
  }

  /* Merge the temp arrays back into arr[l..r]*/
  i = 0; // Initial index of first subarray
  j = 0; // Initial index of second subarray
  k = l; // Initial index of merged subarray
  while (i < n1 && j < n2)
  {
    if (L[i]->key <= R[j]->key)
    {
      arr[k] = L[i];
      i++;
    }
    else
    {
      arr[k] = R[j];
      j++;
    }
    k++;
  }

  /* Copy the remaining elements of L[], if there
  are any */
  while (i < n1)
  {
    arr[k] = L[i];
    i++;
    k++;
  }

  /* Copy the remaining elements of R[], if there
  are any */
  while (j < n2)
  {
    arr[k] = R[j];
    j++;
    k++;
  }
}

/* l is for left index and r is right index of the
sub-array of arr to be sorted */
void mergeSort(keyValuePair **arr, int l, int r) // 0, 2
{
  if (l < r)
  {
    int m = l + (r - l) / 2;   // m = 0

    // Sort first and second halves
    mergeSort(arr, l, m); // 0, 0
    mergeSort(arr, m + 1, r); // 1, 2

    merge(arr, l, m, r);
  }
}

void* thrFunc(void* arg){

  thread_data_t *data = (thread_data_t *)arg;
  mergeSort(data->arr, data->l, data->r); 

  pthread_exit(NULL);
}


/**
 * Step 1: parse the input into an array
 * Step 2: segregate the arrays base on input threads
 * Step 3: merge sort the seperate arrays
 * Step 4: create a final array that integrate the segregated arrays
 * Step 5: merge sort the final array 
*/

int main(int argc, char *argv[])
{
  // Check if the program was called with three arguments
  if (argc != 4)
  {
    printf("Usage: %s input_file output_file\n", argv[0]);
    return -1;
  }

  // Open the input file for reading
  FILE *input_file = fopen(argv[1], "r");
  if (input_file == NULL)
  {
    printf("Failed to open input file: %s\n", argv[1]);
    return -1;
  }

/* Step 1: parse the input into an array*/

  // Allocate a 2D array that will store the input file contents
  keyValuePair **lines = malloc(sizeof(keyValuePair *) * MAX_LINES);
  if (lines == NULL)
  {
    printf("lines malloc failed");
    return -1;
  }
  for (int i = 0; i < MAX_LINES; i++)
  {
    lines[i] = malloc(sizeof(keyValuePair) * MAX_LINE_LENGTH);
    lines[i]->value = malloc(sizeof(char) * MAX_LINE_LENGTH);
  }

  // Parse the content of the input file to the allocated array
  int num_lines = 0;
  char buffer[MAX_LINE_LENGTH];

  while (fgets(buffer, MAX_LINE_LENGTH, input_file) != NULL)
  {
    //  Copy the line to the lines array
    lines[num_lines]->key = *(int *)buffer;
    strcpy(lines[num_lines]->value, buffer);
    num_lines++;
  }

/* Step 2: segregate the arrays base on number of input threads*/

  int num_thread = atoi(argv[3]); // number of threads; given by user
  int lines_each_thread = num_lines / num_thread;

  // case when number of threads is larger than number of lines
  // for example, given 9 lines  and 12 threads, use only 2 threads
  // each thread handles 4 lines
  if(lines_each_thread == 0){
    num_thread = 2;
    lines_each_thread = num_lines / num_thread;
  }
    int remainder = num_lines % num_thread;
    num_lines = num_lines - remainder;

  // the parent array for all the segregate array; 
  // For example, given 10 lines and 2 threads, there will be 2 arrays, 
  // and 5 lines will be merge sorted in each thread
  // { {line 0, line 1, line2, line 3, line 4}, {line 5, line 6, line 7, line 8, line 9} }
  keyValuePair *parent_array[num_thread][lines_each_thread];
  
  int j = 0; // to increment the index of child array
  int k = 0; // to increment the index of thread within child array
  for(int i = 0; i < num_lines; i++){
    parent_array[j][k] = lines[i];
    if(k == lines_each_thread - 1){
      j++;
      k = 0;
      continue;
    }
    k++; 
  }  
  
/* Step 3: merge sort the seperate arrays*/

  // initialize data for thrFunc()
  thread_data_t data[num_thread];
  for(int i = 0; i < num_thread; i++){
    data[i].arr = parent_array[i];
    data[i].l = 0;
    data[i].r = lines_each_thread - 1;
  }
  
  pthread_t thr[num_thread];

  // uint64_t start, end;
  // start = get_current_time();
  // create multiple threads for merge sort
  for(int i = 0; i < num_thread; i++){
    pthread_create(&thr[i], NULL, thrFunc, &data[i]);
  }

  // block until all threads complete
  for (int i = 0; i < num_thread; i++) {
    pthread_join(thr[i], NULL);
  }

  // end = get_current_time();
  // printf("Print time: %lu\n", end-start);

/* Step 4: create a final array that integrate the segregated arrays*/

  // if there are 10 lines and 2 threads, the lines within each thread up till now is sorted, but still need to use this
  // final array to sort all lines 
  keyValuePair *final_array[num_lines];

  // printf("\nfinal array before final merge sort; should be same as previous: \n");
  // copies the child array to the final array
  j = 0;
  k = 0;
  for(int i = 0; i < num_lines; i++){
    final_array[i] = parent_array[j][k];
    if(k == lines_each_thread - 1){
      j++;
      k = 0;
      continue;
    }
    k++; 

  }

  num_lines = num_lines + remainder; 

  if(remainder != 0){
    for(int i = num_lines - remainder; i < num_lines; i++){
      final_array[i] = lines[i];
    }
  }
  
/* Step 5: merge sort the final array */
  mergeSort(final_array, 0, num_lines - 1);

  // Close the input file
  fclose(input_file);

  // Open the output file for writing
  FILE *output_file = fopen(argv[2], "w");
  if (output_file == NULL)
  {
    printf("Failed to open output file: %s\n", argv[2]);
    return 1;
  }

  // Write each line of the input file to the output file
  for (int i = 0; i < num_lines; i++)
  {
    printf("%d\n", final_array[i]->key);
    fprintf(output_file, "%s", final_array[i]->value);
  }

  // Close the output file
  fclose(output_file);
  return 0;
}



  //   void printArray(int lines_each_thread, keyValuePair *arr[][lines_each_thread], int num_lines){
  //   int j = 0;
  //   int k = 0;
  //   for(int i = 0; i < num_lines; i++){
  //     printf("%s", arr[j][k]->value);
  //     if(k == lines_each_thread - 1){
  //       j++;
  //       k = 0;
  //       continue;
  //     }
  //     k++; 
  //   }
  // }


  // if (num_lines >= MAX_LINES)
  // {
  //   printf("Error: too many lines in input file\n");
  //   fclose(input_file);
  //   return 1;
  // }

  // printf("%s", final_array[i]->value);

  // printf("\nfinal array after final merge sort\n");
  // j = 0;
  // k = 0;
  // for(int i = 0; i < num_lines; i++){
  //   printf("%s", final_array[i]->value);
  //   if(k == lines_each_thread - 1){
  //     j++;
  //     k = 0;
  //     continue;
  //   }
  //   k++; 
  // }
