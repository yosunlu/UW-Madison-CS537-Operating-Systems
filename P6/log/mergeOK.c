#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINES 1000
#define MAX_LINE_LENGTH 1000

typedef struct keyValuePair
{
  int key;
  char *value;
} keyValuePair;

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

  keyValuePair *arr[9];

  int t = 0; // increments temp array
  while (fgets(buffer, MAX_LINE_LENGTH, input_file) != NULL)
  {
    if (num_lines >= MAX_LINES)
    {
      printf("Error: too many lines in input file\n");
      fclose(input_file);
      return 1;
    }
    //  Copy the line to the lines array
    // strcpy((char *)lines[num_lines], buffer);
    (arr[t]) = malloc(sizeof(keyValuePair*));
    lines[num_lines]->key = *(int *)buffer;
    strcpy(lines[num_lines]->value, buffer);
    arr[t] = lines[num_lines];
    t++;
    num_lines++;
  }

  int num_thread = atoi(argv[3]); // number of threads; given by user
  int lines_each_thread = num_lines / num_thread;

  
  
  mergeSort(arr, 0, num_lines - 1);   // 0, 4
  
  for (int i = 0; i < 9; i++)
  {
    printf("%d\n", arr[i]->key);
    printf("%s\n", arr[i]->value);
  }

  // Close the input file
  // fclose(input_file);

  // // Open the output file for writing
  // FILE *output_file = fopen(argv[2], "w");
  // if (output_file == NULL)
  // {
  //   printf("Failed to open output file: %s\n", argv[2]);
  //   return 1;
  // }

  // // Write each line of the input file to the output file
  // for (int i = 0; i < num_lines; i++)
  // {
  //   fprintf(output_file, "%s", (char *)lines[i]);
  // }

  // // Close the output file
  // fclose(output_file);

  // printf("File copied successfully from %s to %s\n", argv[1], argv[2]);

  return 0;
}