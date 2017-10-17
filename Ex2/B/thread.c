#include <stdio.h>
#include  <sys/types.h>
 
int global = 0;

void increment(int local){
    int i;
  for (i = 0; i < 10; i++){
    global ++;
    local ++;
    printf ("value of local = %d\n", local);
    printf ("value of global = %d\n", global);
  }

}

int main () {
  int local = 0;

  pthread_t t1;
  pthread_t t2;
  pthread_create(&t1, NULL, increment, local);
  pthread_create(&t2, NULL, increment, local);
  pthread_join(t1, NULL);
  pthread_join(t2, NULL);

  return 0;
}

//Inkrementerer hver sin kopi av local.
//Begge incrementerer samme global.