
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

void main()
{
    int fp = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC); // 要overwrite一定要有TRUNC FLAG
    if (fp == -1)
    {
        fprintf(stdout, "%s", "ERROR: Can't open file\n");
        exit(1);
    }
    dup2(fp, 1); // \\\每一個(代表各自獨立)/// process開始前就有的： 0: STDIN, 1: STDOUT, 2: STDERR; 這裡做的是把STDOUT的東西轉印到創建的file descriptor裏
    char *const parm[] = {"ls", NULL};
    execv("/bin/ls", parm);
}