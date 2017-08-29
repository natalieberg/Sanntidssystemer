#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/times.h>
#include <unistd.h>

void busy_wait_delay(int seconds)
{
	int i, dummy;
	int tps = sysconf(_SC_CLK_TCK);
	clock_t start;
	struct tms exec_time;
	times(&exec_time);
	start = exec_time.tms_utime;

	while( (exec_time.tms_utime-start) < (seconds * tps))
	{
		for(i=0; i<1000; i++)
		{
			dummy = i;
		}
		times(&exec_time);
	}
}

void *t1Print(void *vargp)
{
	printf("t1 start \n");
	busy_wait_delay(5);
	printf("t1 done sleeping \n");
}

void *t2Print(void *vargp)
{
	printf("t2 start \n");
	busy_wait_delay(5);
	printf("t2 done sleeping \n");
}

int main()
{
    pthread_t t1;
    pthread_t t2;
    pthread_create(&t1, NULL, t1Print, NULL);
    pthread_join(t1, NULL);
    pthread_create(&t2, NULL, t2Print, NULL);
    pthread_join(t2, NULL);
    exit(0);
}