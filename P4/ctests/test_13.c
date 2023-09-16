#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"
#include "syscall.h"

int
main(int argc, char *argv[]){
	int pid_par = getpid();
	
	if(settickets(5) == 0)
	{
	}
	else
	{
	 printf(1, "XV6_SCHEDULER\t FAILED\n");
	 exit();
	}
	
	if(fork() == 0){
		if(settickets(3) != 0){
	 		printf(1, "XV6_SCHEDULER\t FAILED\n");
	 		exit();
		}
		int pid_chd = getpid();
		int init_pass = -1;
		for(int j = 0; j < 100; j++){
			struct pstat st;
			if(getpinfo(&st) != 0)
			{
				printf(1, "XV6_SCHEDULER\t FAILED\n");
				exit();
			}
			for(int i = 0; i < NPROC; i++){
      				if (st.pid[i] == pid_par){
				}
				else if (st.pid[i] == pid_chd){
					if(init_pass == -1){
						init_pass = st.pass[i];
					}
					if((st.pass[i] - init_pass) % st.strides[i] != 0){
						printf(1, "XV6_SCHEDULER\t FAILED\n");
						exit();
					}
				}
			}
			sleep(1);
		}

    		exit();
	}
  	while(wait() > 0);
	printf(1, "XV6_SCHEDULER\t SUCCESS\n");
	exit();
}
