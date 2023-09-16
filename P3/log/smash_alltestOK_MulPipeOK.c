#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

const char error_message[30] = "An error has occurred\n";
struct command{
    char **argv;
};
/**
 * Execute built-in commands: exit, cd, pwd
 */
int exec_builtin(char ***args, int *num_args)
{
    if (strcmp((*args)[0], "exit") == 0)
    {
        if (*num_args != 1) 
            return -1;
        exit(0);
    }
    else if (strcmp((*args)[0], "cd") == 0)
    {
        if (*num_args != 2)
            return -1;
        if (chdir((*args)[1]) == -1)
            return -1;
        return 0; // cd success
    }
    else if (strcmp((*args)[0], "pwd") == 0)
    {
        if (*num_args != 1)
            return -1;

        size_t size = 1;
        char *buf;
        buf = malloc(size);
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

int
spawn_proc (int in, int out, struct command *cmd)
{
  pid_t pid;

  if ((pid = fork ()) == 0)
    {
      if (in != 0)
        {
          dup2 (in, 0);
          close (in);
        }

      if (out != 1)
        {
          dup2 (out, 1);
          close (out);
        }

      return execvp (cmd->argv [0], (char * const *)cmd->argv);
    }

  return pid;
}

/**
 * redirect STDOUT to output file
 */
int redirect(char ***args, int num_args)
{

    char **const parmList = malloc(sizeof(char *) * (num_args - 1)); // store the arguments before >
    for (int i = 0; i < (num_args - 2); i++)
    { // iterate to the arg before >, and store in parmList
        parmList[i] = (*args)[i];
    }
    parmList[num_args - 2] = NULL; // the last element of parameter array must be null; see execv man page
    int fd = open((*args)[num_args - 1], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    if (fd == -1)
        return -1;
    dup2(fd, 1); // redirect STDOUT to new file descriptor
    if (-1 == execv(parmList[0], parmList))
        return -1;

    return 0;
}

int
fork_pipes (int n, struct command *cmd, char*** args, int numargs)
{
  int i;
  pid_t pid;
  int in, fd [2];

  /* The first process should get its input from the original file descriptor 0.  */
  in = 0;

  /* Note the loop bound, we spawn here all, but the last stage of the pipeline.  */
  for (i = 0; i < n - 1; ++i)
    {
      pipe (fd);

      /* f [1] is the write end of the pipe, we carry `in` from the prev iteration.  */
      spawn_proc (in, fd [1], cmd + i);

      /* No need for the write end of the pipe, the child will write here.  */
      close (fd [1]);

      /* Keep the read end of the pipe, the next child will read from there.  */
      in = fd [0];
    }

  /* Last stage of the pipeline - set stdin be the read end of the previous pipe
     and output to the original file descriptor 1. */  

  if (in != 0){
        if(strcmp((*args)[numargs - 2], ">") == 0){
            dup2(in, 0);
            int i = 0;
            while(strcmp(cmd[n-1].argv[i], ">") != 0){
                // printf("%s\n",cmd[n-1].argv[i] );
                i++;
                if(strcmp(cmd[n-1].argv[i], ">") == 0){
                    cmd[n-1].argv[i] = NULL;
                    break;
                }
            }
            
            int fd = open(cmd[n-1].argv[i+1], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
            if (fd == -1)
                return -1;
        
            dup2(fd, 1);
            if (execv(cmd[n-1].argv[0], cmd[n-1].argv) == -1)
                    return -1;
            return 0;
  }
    dup2 (in, 0);
  }

  /* Execute the last stage with the current process. */
  return execvp (cmd [i].argv [0], (char * const *)cmd [i].argv);
}


int piping(char ***args, int *num_args, int pipe_index)
{
    struct command cmd[10];

    int c = 0; // index of command
    int j = 0; // index of argument within command
    cmd[c].argv = (char **) malloc(sizeof(char *) * 10);

    for(int i = 0; i < *num_args; i++){
        if(strcmp((*args)[i], "|") == 0){
            cmd[c].argv[j] = NULL;
            c++;
            j = 0;
            cmd[c].argv = (char **) malloc(sizeof(char *) * 10);
            continue;
        }
        cmd[c].argv[j] = (char *) malloc(sizeof(char) * 256);
        strcpy(cmd[c].argv[j], (*args)[i]);
        // printf("%s\n",cmd[c].argv[j]);
        j++;
    }
    cmd[c].argv[j] = NULL;
    
    return fork_pipes(c+1, cmd, args, *num_args);

    // char **beforePipe;
    // char **afterPipe;
    // beforePipe = malloc(sizeof(char *) * 10);
    // for (int i = 0; i < 10; i++)
    // {
    //     beforePipe[i] = malloc(256);
    // }
    // afterPipe = malloc(sizeof(char *) * 10);
    // for (int i = 0; i < 10; i++)
    // {
    //     afterPipe[i] = malloc(256);
    // }
    // int beforeCount = 0;
    // int afterCount = 0;

    // for (int i = 0; i < pipe_index; i++)
    // {
    //     strcpy(beforePipe[i], (*args)[i]);
    //     beforeCount++;
    // }

    // beforePipe[beforeCount] = NULL;

    // int j = 0;
    // for (int i = pipe_index + 1; i < *num_args; i++)
    // {
    //     strcpy(afterPipe[j], (*args)[i]);
    //     afterCount++;
    //     j++;
    // }

    // afterPipe[afterCount] = NULL;

    // int pipefd[2];
    // pipe(pipefd); // 0 = read end ; 1 = write end
    // int pid1 = fork();
    // if (pid1 == 0)
    // {
    //     dup2(pipefd[1], 1); // 0: STDIN, 1: STDOUT, 2: STDERR
    //     close(pipefd[0]);
    //     close(pipefd[1]);
    //     if (execv(beforePipe[0], beforePipe) == -1)
    //         return -1;
    // }

    // int pid2 = fork();
    // if (pid2 == 0)
    // {
    //     dup2(pipefd[0], 0);
    //     // if redirection is needed
    //     if(afterCount > 1){
    //         if (strcmp(afterPipe[afterCount - 2], ">") == 0)
    //         {
    //             close(pipefd[0]);
    //             close(pipefd[1]);

    //             // parse the arguments after pipe before >
    //             char **const parmList = malloc(sizeof(char *) * (afterCount - 1));
    //             for (int i = 0; i < (afterCount - 2); i++)
    //             { // iterate to the arg before >, and store in parmList
    //                 parmList[i] = afterPipe[i];
    //             }
    //             parmList[afterCount - 2] = NULL;
    //             // open the file named after the last argument
    //             int fd = open(afterPipe[afterCount - 1], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    //             if (fd == -1)
    //                 return -1;
    //             dup2(fd, 1);
    //             if (execv(parmList[0], parmList) == -1)
    //                 return -1;

    //             return 0;
    //         }
    //     }
    //     close(pipefd[0]);
    //     close(pipefd[1]);
    //     if (execv(afterPipe[0], afterPipe) == -1)
    //     {
    //         return -1;
    //     }
    // }
    // close(pipefd[0]);
    // close(pipefd[1]);
    // waitpid(pid1, NULL, 0);
    // waitpid(pid2, NULL, 0);
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
        if (strcmp((*args)[i], "|") == 0) // piping needed                               
            return piping(args, num_args, i); // i is the index of "|"
    }
    // check second if redirection needed
    for (int i = 1; i < *num_args; i++)
    {
        if (strcmp((*args)[i], ">") == 0)
        { // redirection needed
            if (strcmp((*args)[*num_args - 2], ">") != 0) // if > provided, it has to be the second last argument
                return -1;
            return redirect(args, *num_args);
        }
    }
    // redirection not needed
    (*args)[*num_args] = NULL; // the last element of array of parameters must be null
    if (-1 == execv((*args)[0], *args))
        return -1;

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
        return -1;
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
            return -1;

        (*args)[(*num_args)++] = token_copy;
        if (strcmp(token_copy, ";") == 0)
            ret++;
        token = strtok(NULL, " \t\n");
    }
    return ret;
}

int main()
{
    char *buf = NULL;
    size_t size;
    char **args;
    int *args_num;
    args_num = malloc(sizeof(int));
    
    while (1)
    {
        printf("smash> ");
        fflush(stdout);
        getline(&buf, &size, stdin);

        int delim_num = lexer(buf, &args, args_num); // number of ";""
        if (delim_num == -1){
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
        int argnumArray[cmds_num]; // each element of the array store the number of arguments of each command

        // store the commands, for example:
        // pwd ; /bin/ls -la ; /bin/cat temp > output
        // before the ; is met, pwd will be stored in cmds[0][0]. if ; is met, argnumArray[0] will be set to 1
        // argcount ++ so  argnumArray[1] will store the number of arguments of next command
        // count = 0 to reset the number of argument
        // when the next ; is met, argnumArray[1] will store 2
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
                    write(STDERR_FILENO, error_message, strlen(error_message)); 
                    continue;
                }

                if (endptr == str)
                {
                    write(STDERR_FILENO, error_message, strlen(error_message)); 
                    continue;
                }
                numLoop = val;

                for (int j = 0; j < argnumArray[i] - 2; j++)
                    cmds[i][j] = cmds[i][j + 2]; // store the arguments after "loop x"
                    
                argnumArray[i] = argnumArray[i] - 2;
            }

            // start executing the command; numLoop == 1 if no loop
            for (int j = 0; j < numLoop; j++)
            {
                // built-in commands
                if (strcmp(cmds[i][0], "exit") == 0 || strcmp(cmds[i][0], "cd") == 0 || strcmp(cmds[i][0], "pwd") == 0)
                {
                    if (exec_builtin(&cmds[i], &argnumArray[i]) == -1)
                        write(STDERR_FILENO, error_message, strlen(error_message));
                }
                else  // not built-in command
                {
                    pid_t my_pid = fork();
                    if (my_pid < 0)
                        write(STDERR_FILENO, error_message, strlen(error_message));
                    else if (my_pid == 0)
                    {   
                        if (exec_prog(&cmds[i], &argnumArray[i]) == -1)
                            write(STDERR_FILENO, error_message, strlen(error_message));
                        exit(0);
                    }
                    wait(NULL);
                }
            }
        }
        // for (int i = 0; i < cmds_num; i++)
        // {
        //     for (int j = 0; j < 10; j++)
        //     {
        //         free(*(*(cmds + i) + j));
        //     }
        //     free (*(cmds + i));
        // }
        // free(cmds);
        // free(buf);
        // free(args);
        // free(args_num);
    }
}
