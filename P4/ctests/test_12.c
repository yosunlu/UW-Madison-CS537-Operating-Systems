#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "pstat.h"

void print_pinfo(struct pstat* pstat, int child) {
    printf(1, "Printing for child: %d\n", child);
    for (int i = 0; i < NPROC; i++) {
        if (!pstat->inuse[i]) continue;
        printf(1, "pid: %d, tickets: %d, stride: %d, pass: %d, ticks: %d\n", 
                pstat->pid[i], pstat->tickets[i], pstat->strides[i], pstat->pass[i], pstat->ticks[i]);
    }
}

const int N = 16000000;

int
main(int argc, char *argv[]) {
  struct pstat pstat;

  int pid1, pid2;

  float counter = 0;

  pid1 = fork();
  if (pid1 < 0) {
    printf(2, "Fork child process 1 failed\n");
  } else if (pid1 == 0) { // child process 1
    settickets(3);
    while (counter < N) counter += 1;
    getpinfo(&pstat);
    print_pinfo(&pstat, 1);
    printf(1, "Child process 1 finished\n");
    exit();
  } else {
    pid2 = fork();
    if (pid2 < 0) {
        printf(2, "Fork child process 2 failed\n");
        exit();
    } else if (pid2 == 0) { // child process 2
        settickets(2);
        while (counter < N) counter += 1;
        printf(1, "Child process 2 finished\n");
        getpinfo(&pstat);
        print_pinfo(&pstat, 2);
        exit();
    }
  }
  
  wait();
  wait();
  printf(1, "XV6_SCHEDULAR\t SUCCESS\n");
  exit();
}
