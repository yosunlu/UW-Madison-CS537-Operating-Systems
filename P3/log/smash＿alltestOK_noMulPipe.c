#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

const char error_message[30] = "An error has occurred\n";
/**
 * Execute built-in commands: exit, cd, pwd
 */
int exec_builtin(char ***args, int *num_args)
{
    if (strcmp((*args)[0], "exit") == 0)
    {
        // printf("123");
        if (*num_args != 1)
        {
            // printf("No argument(s) should follow exit\n");
            return -1;
        }
        exit(0);
    }
    else if (strcmp((*args)[0], "cd") == 0)
    {
        if (*num_args != 2)
        {
            // printf("Usage: cd /path\n");
            return -1;
        }
        if (chdir((*args)[1]) == -1)
        {
            // printf("No such file or directory\n");
            return -1;
        }
        return 0; // cd success
    }
    else if (strcmp((*args)[0], "pwd") == 0)
    {
        if (*num_args != 1)
        {
            // printf("No argument(s) should follow pwd\n");
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
        fflush(stdout);
        printf("%s\n", buf);
        free(buf);
        return 0; // pwd success
    }
    return -1; // should not reach
}

/**
 * redirect STDOUT to output file
 */
int redirect(char ***args, int *num_args)
{

    char **const parmList = malloc(sizeof(char *) * (*num_args - 1)); // store the arguments before >
    for (int i = 0; i < (*num_args - 2); i++)
    { // iterate to the arg before >, and store in parmList
        parmList[i] = (*args)[i];
    }
    parmList[*num_args - 2] = NULL; // the last element of parameter array must be null; see execv man page
    int fd = open((*args)[*num_args - 1], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    if (fd == -1)
        return -1;
    dup2(fd, 1); // redirect STDOUT to new file descriptor
    if (-1 == execv(parmList[0], parmList))
        return -1;

    return 0;
}

int piping(char ***args, int *num_args, int pipe_index)
{


    char **beforePipe;
    char **afterPipe;
    beforePipe = malloc(sizeof(char *) * 10);
    for (int i = 0; i < 10; i++)
    {
        beforePipe[i] = malloc(256);
    }
    afterPipe = malloc(sizeof(char *) * 10);
    for (int i = 0; i < 10; i++)
    {
        afterPipe[i] = malloc(256);
    }
    int beforeCount = 0;
    int afterCount = 0;

    for (int i = 0; i < pipe_index; i++)
    {
        strcpy(beforePipe[i], (*args)[i]);
        // printf("%s\n", (*args)[i]);
        beforeCount++;
    }

    beforePipe[beforeCount] = NULL;

    int j = 0;
    for (int i = pipe_index + 1; i < *num_args; i++)
    {
        strcpy(afterPipe[j], (*args)[i]);
        // printf("%s\n", (*args)[i]);
        afterCount++;
        j++;
    }

    afterPipe[afterCount] = NULL;

    int pipefd[2];
    pipe(pipefd); // 0 = read end ; 1 = write end
    int pid1 = fork();
    if (pid1 == 0)
    {
        printf("%ld\n", (long)getpid());
        dup2(pipefd[1], 1); // 0: STDIN, 1: STDOUT, 2: STDERR
        close(pipefd[0]);
        close(pipefd[1]);
        if (execv(beforePipe[0], beforePipe) == -1)
        {
            // printf("beforePipefailure\n");
            return -1;
        }
      
    }

    int pid2 = fork();
    if (pid2 == 0)
    {
        printf("%ld\n", (long)getpid());
        dup2(pipefd[0], 0);
        // if redirection is needed
        if(afterCount > 1){
            if (strcmp(afterPipe[afterCount - 2], ">") == 0)
            {
                close(pipefd[0]);
                close(pipefd[1]);

                // parse the arguments after pipe before >
                char **const parmList = malloc(sizeof(char *) * (afterCount - 1));
                for (int i = 0; i < (afterCount - 2); i++)
                { // iterate to the arg before >, and store in parmList
                    parmList[i] = afterPipe[i];
                    // printf("%s\n", parmList[i]);
                }
                parmList[afterCount - 2] = NULL;
                // open the file named after the last argument
                int fd = open(afterPipe[afterCount - 1], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
                if (fd == -1)
                    return -1;
                dup2(fd, 1);
                if (execv(parmList[0], parmList) == -1)
                {
                    // printf("beforePipefailure\n");
                    return -1;
                }
                return 0;
            }
        }
        close(pipefd[0]);
        close(pipefd[1]);
        if (execv(afterPipe[0], afterPipe) == -1)
        {
            // printf("beforePipefailure\n");
            return -1;
        }
    }
    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    // wait(NULL);

    return 0;
}
/**
 * Execute non-built-in commands
 */
int exec_prog(char ***args, int *num_args)
{

    // check first if piping is needed

    for (int i = 1; i < *num_args; i++)
    {
        if (strcmp((*args)[i], "|") == 0)
        {                                     // piping needed
            return piping(args, num_args, i); // i is the index of "|"
        }
    }

    // check second if redirection needed
    for (int i = 1; i < *num_args; i++)
    {
        if (strcmp((*args)[i], ">") == 0)
        { // redirection needed
            if (strcmp((*args)[*num_args - 2], ">") != 0)
            { // if > provided, it has to be the second last operator
                // printf("> not at correct position\n");
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
    int ret = 0; // counter for ";" ; ret = -1 on failure
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
        if (strcmp(token_copy, ";") == 0)
        {
            ret++;
        }
        token = strtok(NULL, " \t\n");
    }
    return ret;
}

int main()
{
    
    while (1)
    {
        char *buf = NULL;
        size_t size;
        char **args = malloc(sizeof(int));
        int *args_num = malloc(sizeof(int));

        fflush(stdout);
        printf("smash> ");
        fflush(stdout);
        getline(&buf, &size, stdin);

        int delim_num = lexer(buf, &args, args_num); // number of ";""
        if (delim_num == -1){
            // fprintf(stderr, "parsing error\n");
            write(STDERR_FILENO, error_message, strlen(error_message)); 
        }

        // separate the commands with delimiter ";", and store in separate char**
        int cmds_num = delim_num + 1; // number of commands
        char ***cmds;
        cmds = malloc(sizeof(char **) * cmds_num);
        for (int i = 0; i < cmds_num; i++)
        {
            *(cmds + i) = malloc(256);
            for (int j = 0; j < 10; j++)
            {
                *(*(cmds + i) + j) = malloc(256);
            }
        }

        int argcount = 0; // the index of the command
        int count = 0;    // the index of the arguement within command
        int argnumArray[cmds_num];

       // store the commands in d
        for (int i = 0; i < *args_num; i++)
        {
            if (strcmp(args[i], ";") == 0)
            {
                argnumArray[argcount] = count;
                argcount++;
                count = 0;
                continue;
            }
            strcpy(cmds[argcount][count], args[i]);
            count++;
        }
        argnumArray[cmds_num - 1] = count;

        // start examining each commands

        for (int i = 0; i < cmds_num; i++)
        {
            // skip 0 argument commands, ex. pwd ; ; /bin/ls
            if (argnumArray[i] == 0)
                continue;

            // check if loop is needed
            int numLoop = 1; // if command has no loop, execute once

            if (strcmp(cmds[i][0], "loop") == 0)
            {
                // check if the argument after loop is a integer
                int base = 10;
                char *endptr, *str;
                long val;
                str = cmds[i][1];
                errno = 0;

                val = strtol(str, &endptr, base); // parse the number of loop needed

                // check for various possible errors
                if (errno != 0)
                {
                    // fprintf(stderr, "errno for strtol\n");
                    write(STDERR_FILENO, error_message, strlen(error_message)); 
                    continue;
                }

                if (endptr == str)
                {
                    // fprintf(stderr, "No digits were found after loop\n");
                    write(STDERR_FILENO, error_message, strlen(error_message)); 
                    continue;
                }
                numLoop = val;
                // printf("%d\n", argnumArray[i]);
                for (int j = 0; j < argnumArray[i] - 2; j++)
                {
                    cmds[i][j] = cmds[i][j + 2];
                }
                argnumArray[i] = argnumArray[i] - 2;
            }

            // start executing the command; j == 1 if no loop
            for (int j = 0; j < numLoop; j++)
            {
                // built-in commands
                if (strcmp(cmds[i][0], "exit") == 0 || strcmp(cmds[i][0], "cd") == 0 || strcmp(cmds[i][0], "pwd") == 0)
                {
                    if (exec_builtin(&cmds[i], &argnumArray[i]) == -1)
                    {
                        // fprintf(stderr, "built-in program error\n");

                        write(STDERR_FILENO, error_message, strlen(error_message)); 
                    };
                }
                else
                {
                    char path[6];
                    strncpy(path, cmds[i][0], 5);
                    path[6] = '\0';
                    if(strcmp(path, "/bin/") != 0){
                        write(STDERR_FILENO, error_message, strlen(error_message));
                        break;
                    }
                    // not built-in command
                    // printf("parent pid: %ld\n", (long)getpid());
                    pid_t my_pid = fork();
                    if (my_pid < 0)
                    {
                        // fprintf(stderr, "fork failed\n");
                        write(STDERR_FILENO, error_message, strlen(error_message));
                    }
                    else if (my_pid == 0)
                    {   

                        // printf("child pid: %ld\n", (long)getpid());
                        if (exec_prog(&cmds[i], &argnumArray[i]) == -1)
                        {
                            // fprintf(stderr, "non built-in program error\n");
                            write(STDERR_FILENO, error_message, strlen(error_message));
                        }
                        exit(0);
                    }
                    wait(NULL);
                }
                
            }
            
        }
    }
}
