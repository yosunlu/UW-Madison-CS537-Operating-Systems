#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"
#include "syscall.h"

int
main(int argc, char *argv[]){

	int orig = fork();
	if(fork() == 0) orig = 0;
	if(fork() == 0) orig = 0;
	if(fork() == 0) orig = 0;
	int p;
	if((p = fork()) == 0) orig = 0;
	int pid = getpid();
	
	struct pstat st;
        if(getpinfo(&st) != 0)
        {
        	printf(1, "XV6_SCHEDULER\t FAILED\n");
                exit();
        }
	for(int i = 0; i < NPROC; i++){
      		if (st.pid[i] == pid){
			if(st.tickets[i] != 1){
        			printf(1, "XV6_SCHEDULER\t FAILED\n");
				exit();
			}
			if(st.strides[i] == 0){
				printf(1, "XV6_SCHEDULER\t FAILED\n");
				exit();
			}
		}
	}
	
	if(p != 0){
		while(wait() > 0);
	}
	if(orig != 0){	
		printf(1, "XV6_SCHEDULER\t SUCCESS\n");
	}
	exit();
}
