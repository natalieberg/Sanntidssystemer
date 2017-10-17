#include <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

int global = 0;

int main () {

 
  int local = 0;

  pid_t pid;
 
  vfork();
  pid = getpid();

  int i;
  for (i = 0; i < 10; i++){
  	global ++;
  	local ++;
  	printf ("value of local = %d, pid = %d\n", local, pid);
  	printf ("value of global = %d, pid = %d\n", global, pid);
  }

  _exit(2);

  return 0;
}

//Parent is suspended until child terminates.
//Child shares all memory with parent, including stack.
//Child must not return from current function or call exit().