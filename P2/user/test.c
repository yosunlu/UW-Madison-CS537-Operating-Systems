#include "types.h"
#include "stat.h"
#include "user.h"

void
test()
{
	char state[16];
	int ret = getprocstate(getpid(), state, 16);
        
	if(ret == 0 && strcmp(state, "run   ") == 0) {
		printf(1, "TEST PASSED");
	} else {
        printf(1, "ret = %d\nstate = %s\n", ret, state);
		printf(1, "TEST FAILED got proc state %s, expected %s", state, "run   ");
	}

	exit();
}

