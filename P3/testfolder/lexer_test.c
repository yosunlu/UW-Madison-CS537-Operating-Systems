#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int lexer(char *line, char ***args, int *num_args){

    *num_args = 0;
    // count number of args
    char *l = strdup(line);
    if(l == NULL){
        return -1;
    }
    char *token = strtok(l, " \t\n");
    while(token != NULL){
        (*num_args)++;
        token = strtok(NULL, " \t\n");
    }
    free(l);
    // split line into args
    *args = malloc(sizeof(char **) * *num_args);
    *num_args = 0;
    token = strtok(line, " \t\n");
    while(token != NULL){
        char *token_copy = strdup(token);
        if(token_copy == NULL){
            return -1;
        }
        (*args)[(*num_args)++] = token_copy;
        // printf("%s\n", (*args)[(*num_args)-1]);
        token = strtok(NULL, " \t\n");
    }
    return 0;
}

int main(){

    printf("input: ");
    char *buf = NULL;
    size_t size;
    getline(&buf, &size, stdin);
    
    char ***args = malloc(sizeof(int));
    int* args_num = malloc(sizeof(int));
    lexer(buf, args, args_num);
    printf("%d\n", *args_num);
    printf("%s\n", (*args)[1]);
    // char **userInput = malloc(sizeof(char*) * 100);
    // for(int i = 0; i < 100; i++){
    //         userInput[i] = malloc(50);
    // }
    // int num_args = 0;

    // char* token = strtok(buf, " \t\n");

    // while(token != NULL){
    //     num_args++;
    //     token = strtok(NULL, " \t\n");
    // }

    // printf("%d", num_args);
    // return 0;
}