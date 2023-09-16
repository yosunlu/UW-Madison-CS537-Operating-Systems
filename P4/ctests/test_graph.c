#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "pstat.h"

void spin()
{
	int i = 0;
  int j = 0;
  int k = 0;
	for(i = 0; i < 50; ++i)
	{
		for(j = 0; j < 10000000; ++j)
		{
      k = j % 10;
      k = k + 1;
    }
	}
}

int
main(int argc, char *argv[]) {
  settickets(4);
  if (fork() == 0) { // child process 1
    settickets(3);
    spin();
    exit();
  } else {
    if (fork() == 0) { // child process 2
      settickets(2);
      spin();
      exit();
    }
  }
  spin(); // main process
  
  wait();
  wait();
  exit();
}
