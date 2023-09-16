#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"
#include "syscall.h"

int
main(int argc, char *argv[]){
	int pid_par = getpid();
	
	struct pstat st;
	if(getpinfo(&st) != 0)
	{
		printf(1, "XV6_SCHEDULER\t FAILED\n");
		exit();
	}
	for(int i = 0; i < NPROC; i++){
		if (st.pid[i] == pid_par){
			if(st.tickets[i] != 1){
        			printf(1, "XV6_SCHEDULER\t FAILED\n");
			}
		}
	}
	

	printf(1, "XV6_SCHEDULER\t SUCCESS\n");
	exit();
}
