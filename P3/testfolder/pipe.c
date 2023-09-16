
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

void main()
{
    dup2(1, 0); // \\\每一個(代表各自獨立)/// process開始前就有的： 0: STDIN, 1: STDOUT, 2: STDERR; 這裡做的是把STDOUT的東西轉印到創建的file descriptor裏
    printf("1\n2\n3\n4");
    char *const parm[] = {"sort", NULL};
    execv("/bin/sort", parm);
}