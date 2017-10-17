#include "io.h"
#include <unistd.h>
#include  <sys/types.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>

#define TESTS 3
#define DISTURBANCES 10


void timespec_add_us(struct timespec *t, long us)
{
// add microseconds to timespecs nanosecond counter
	t->tv_nsec += us*1000;
// if wrapping nanosecond counter, increment second counter
	if (t->tv_nsec > 1000000000)
	{
		t->tv_nsec = t->tv_nsec-1000000000;
		t->tv_sec += 1;
	}
}

int set_cpu(int cpu_number)
{
	// setting cpu set to the selected cpu
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpu_number, &cpu);
	// set cpu set to current thread and return
	return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

void *test(int test)
{
	set_cpu(1);
	while(1)
	{
		if (io_read(test) == 0)
		{
			io_write(test,0);
			usleep(1);
			io_write(test,1);
		}

		usleep(1);
	}
}


void *disturbance(void *vargp)
{
	set_cpu(1);
	int dummy;
	while(1)
	{
		dummy = 0;
		dummy++; 
	}
}

void *periodicPrint(void *vargp)
{ 
	struct timespec next;
	while(1){
		clock_gettime(CLOCK_REALTIME, &next);
		timespec_add_us(&next, 1000);
		printf("YO! \n");
		clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &next, NULL);
	}
}

void *periodicPoll(int test)
{ 
	struct timespec next;
	set_cpu(1);
	while(1){
		clock_gettime(CLOCK_REALTIME, &next);
		timespec_add_us(&next, 100);
		if (io_read(test) == 0)
		{
			io_write(test,0);
			usleep(1);
			io_write(test,1);
		}
		clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &next, NULL);
	}
}


int main()
{

/*
	pthread_t testThreads[TESTS];
	pthread_t disturbanceThreads[DISTURBANCES];

	io_init();

	int i;

	for (i=0; i<TESTS; i++)
	{
		pthread_create(&testThreads[i], NULL, test, i+1);
	}

	for (i=0; i<DISTURBANCES; i++)
	{
		pthread_create(&disturbanceThreads[i], NULL, disturbance, NULL);
	}


	for (i=0; i<TESTS; i++)
	{
		pthread_join(testThreads[i], NULL);
	}

	for (i=0; i<DISTURBANCES; i++)
	{
		pthread_join(disturbanceThreads[i], NULL);
	}

*/
	pthread_t testThreads[TESTS];
	pthread_t disturbanceThreads[DISTURBANCES];

	io_init();
	int i;

	for (i=0; i<TESTS; i++)
	{
		pthread_create(&testThreads[i], NULL, periodicPoll, i+1);
	}

	for (i=0; i<DISTURBANCES; i++)
	{
		pthread_create(&disturbanceThreads[i], NULL, disturbance, NULL);
	}


	for (i=0; i<TESTS; i++)
	{
		pthread_join(testThreads[i], NULL);
	}

	for (i=0; i<DISTURBANCES; i++)
	{
		pthread_join(disturbanceThreads[i], NULL);
	}


	return 0;

}