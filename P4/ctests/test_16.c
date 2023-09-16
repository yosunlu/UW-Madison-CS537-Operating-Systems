#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"
#include "syscall.h"

int
main(int argc, char *argv[]){
		
	int p = fork();
	int parent_pid = getpid();
	int child_pid = 0;
	if(p == 0){
		settickets(6);
		child_pid = getpid();
	}
	else{
		settickets(1);
		child_pid = p;
	}

	int sum = 0;
	for(int i = 0; i < 10000; i++){
		for(int j = 0; j < 10000; j++){
			sum++;
		}
	}

	if(p == 0){
		printf(1, "XV6_SCHEDULER\t SUCCESS\n");
		kill(parent_pid);
		exit();
	}
	else{
		if(wait() > 0){
			exit();
		}
		printf(1, "XV6_SCHEDULER\t FAILED\n");
		kill(child_pid);
		while(wait() > 0);
	}
	exit();	
}
