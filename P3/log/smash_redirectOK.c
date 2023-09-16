#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

/**
 * Execute built-in commands: exit, cd, pwd
 */
int exec_builtin(char ***args, int *num_args)
{
    if (strcmp((*args)[0], "exit") == 0)
    {
        if (*num_args != 1)
        {
            printf("No argument(s) should follow exit\n");
            return -1;
        }
        exit(0);
    }
    else if (strcmp((*args)[0], "cd") == 0)
    {
        if (*num_args != 2)
        {
            printf("Usage: cd /path\n");
            return -1;
        }
        if (chdir((*args)[1]) == -1)
        {
            printf("No such file or directory\n");
            return -1;
        }
        return 0; // cd success
    }
    else if (strcmp((*args)[0], "pwd") == 0)
    {
        if (*num_args != 1)
        {
            printf("No argument(s) should follow pwd\n");
            return -1;
        }
        size_t size = 1;
        char *buf = malloc(size);
        while (getcwd(buf, size) == NULL)
        { // error handling
            if (errno == 34)
            { // error code when allocated size too small
                size = size * 2;
                if (realloc(buf, size * 2) == NULL)
                    return -1;
            }
            else
                return -1; // error other than 34
        }
        printf("%s\n", buf);
        free(buf);
        return 0; // pwd success
    }
    return -1; // should not reach
}

/**
 * redirect STDOUT to output file
*/
int redirect(char ***args, int *num_args){

    char **const parmList = malloc(sizeof(char *) * (*num_args - 1)); // store the arguments before >
    for (int i = 0; i < (*num_args - 2); i++){ // iterate to the arg before >, and store in parmList
        parmList[i] = (*args)[i];
    }
    parmList[*num_args - 2] = NULL; // the last element of parameter array must be null; see execv man page
    int fd = open((*args)[*num_args - 1], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    if (fd == -1) return -1;
    dup2(fd, 1); // redirect STDOUT to new file descriptor
    if (-1 == execv(parmList[0], parmList)) return -1;

    return 0;
}
/**
 * Execute non-built-in commands
 */
int exec_prog(char ***args, int *num_args)
{
    // first cmd not allowed to be >
    if(strcmp((*args)[0], ">") == 0){
        return -1;
    }
 
    // check first if redirection needed
    for(int i = 1; i < *num_args; i++){
        if(strcmp((*args)[i], ">") == 0){     // redirection needed
            if(strcmp((*args)[*num_args - 2], ">") != 0){ // if > provided, it has to be the second last operator
                printf("> not at correct position\n");
                return -1;
            }
            return redirect(args, num_args);
        }
    }
     // redirection not needed
    (*args)[*num_args] = NULL; // the last element of array of parameters must be null

    if (-1 == execv((*args)[0], *args))
    {
        return -1;
    }
    return 0;
}



/**
 * Provided in Project 3 specification
 * description: Takes a line and splits it into args similar to how argc and argv work in main
 * line:        The line being split up. Will be mangled after completion of the function
 * args:        a pointer to an array of strings that will be filled and allocated with the args from the line
 * num_args:    a point to an integer for the number of arguments in args
 * return:      returns 0 on success, and -1 on failure
 */
int lexer(char *line, char ***args, int *num_args)
{
    *num_args = 0;
    // count number of args
    char *l = strdup(line);
    if (l == NULL)
    {
        return -1;
    }
    char *token = strtok(l, " \t\n");
    while (token != NULL)
    {
        (*num_args)++;
        token = strtok(NULL, " \t\n");
    }
    free(l);
    // split line into args
    *args = malloc(sizeof(char **) * (*num_args + 1));
    *num_args = 0;
    token = strtok(line, " \t\n");
    while (token != NULL)
    {
        char *token_copy = strdup(token);
        if (token_copy == NULL)
        {
            return -1;
        }
        (*args)[(*num_args)++] = token_copy;
        token = strtok(NULL, " \t\n");
    }
    return 0;
}

int main()
{
    while (1)
    {

        char *buf = NULL;
        size_t size;
        char **args = malloc(sizeof(int)); 
        int *args_num = malloc(sizeof(int));

        printf("smash> ");
        getline(&buf, &size, stdin);
        lexer(buf, &args, args_num);

        // built-in commands
        if (strcmp((args)[0], "exit") == 0 || strcmp((args)[0], "cd") == 0 || strcmp((args)[0], "pwd") == 0)
        {
            if (exec_builtin(&args, args_num) == -1)
            {
                fprintf(stderr, "built-in program error\n");
            };
        }else{
      
        // not built-in command
            pid_t my_pid = fork();
            if (my_pid < 0)
            {
                fprintf(stderr, "fork failed\n");
                exit(1);
            }
            else if (my_pid == 0)
            {
                if (exec_prog(&args, args_num) == -1)
                {
                    fprintf(stderr, "non built-in program error\n");
                }
            }
            else
            {
                wait(NULL);
            }
        }
    }
}
