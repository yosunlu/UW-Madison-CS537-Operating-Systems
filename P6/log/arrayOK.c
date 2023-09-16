#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINES 1000
#define MAX_LINE_LENGTH 1000

int main(int argc, char *argv[]) {
  // Check if the program was called with three arguments
  if (argc != 4) {
    printf("Usage: %s input_file output_file\n", argv[0]);
    return -1;
  }

  // Open the input file for reading
  FILE *input_file = fopen(argv[1], "r");
  if (input_file == NULL) {
    printf("Failed to open input file: %s\n", argv[1]);
    return -1;
  }

  // Allocate a 2D array that will store the input file contents
  char **lines = malloc(sizeof(char*) * MAX_LINES);
  if(lines == NULL){
    printf("lines malloc failed");
    return -1;
  }
  for(int i = 0; i < MAX_LINES; i++ ){
    lines[i] = malloc(sizeof(char) * MAX_LINE_LENGTH);
  }

  int num_lines = 0;
  char buffer[MAX_LINE_LENGTH];
  while (fgets(buffer, MAX_LINE_LENGTH, input_file) != NULL) {
    if (num_lines >= MAX_LINES) {
      printf("Error: too many lines in input file\n");
      fclose(input_file);
      return 1;
    }
//  Copy the line to the lines array
    strcpy(lines[num_lines], buffer);
    num_lines++;
  }

  // Close the input file
  fclose(input_file);

  // Open the output file for writing
  FILE *output_file = fopen(argv[2], "w");
  if (output_file == NULL) {
    printf("Failed to open output file: %s\n", argv[2]);
    return 1;
  }

  // Write each line of the input file to the output file
  for (int i = 0; i < num_lines; i++) {
    fprintf(output_file, "%s", lines[i]);
  }

  // Close the output file
  fclose(output_file);

  printf("File copied successfully from %s to %s\n", argv[1], argv[2]);

  return 0;
}