#include <stdio.h>
#include  <sys/types.h>
 
int global = 0;

int main () {

 
  int local = 0;

  pid_t pid;
 
  fork();
  pid = getpid();

  int i;
  for (i = 0; i < 10; i++){
  	global ++;
  	local ++;
  	printf ("value of local = %d, pid = %d\n", local, pid);
  	printf ("value of global = %d, pid = %d\n", global, pid);
  }

  return 0;
}

//Fork creates new, child process.
//After child is created, both processes will execute the next intructions.
//Fork creates copy of parent address space.
//Child and parent have separate address spaces.