Name: Yushan Lu
CS Login: yul
WISC ID: 9084761288
Email: yll437@wisc.edu
All works


# Scheduling

- Added 4 new attributes to `struct proc` in `proc.h`
    - `int init_stride`: the initial stride, calculated by MAXSTRIDE / tickets
    - `int tickets`: tickets provided by user; fixed once given
    - `int passvalue`: the value used to determine which process to execute; the lower the better
    - `int ticks`: number of times this process has been executed

- Added `MAXSTRIDE` in `param.h`: the number should be a random constant

- Added a system call in `proc.c` to set the initial tickets
    - `int settickets(int number)`
    
- Added a system call in `proc.c` to return the information of running processes
    - `int sys_getpinfo(void)`

- Added a new `pstat.h` in the include folder

- Modified `scheduler()` in `proc.c` (major modifications were after acquire and before switch)

- Modified `allocproc()` in `proc.c`: the attributes of proc should be initialized

- Modified `fork()` in `proc.c`: child process should inherit parent's `init_stride`, `tickets`; passvalue should be also set to the initial stride

- miscellaneous: 



# Where to add things when implementing a new system call

- `sysproc.c`: the code itself, if `ptable` is needed; can also be put in `proc.c`
- `syscall.c`: under `array of function pointers to handlers for all the syscalls`
- `defs.h`: under `proc.c`
- `usys.S`: in `user` folder
- `syscall.h`: in `include` folder
- `user.h`: in `user` folder
- `sysfunc.h`

# Where to add things when adding tester user program

- `makefile` under `user` folder
- `test.c` under `user` folder