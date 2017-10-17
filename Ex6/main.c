#include "io.h"
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <native/task.h>
#include <native/timer.h>
#include <sys/mman.h>

#define DISTURBANCES 10
#define TESTS 3

int set_cpu(int cpu_number)
{
	// setting cpu set to the selected cpu
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpu_number, &cpu);
	// set cpu set to current thread and return
	return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

void testTask(int test)
{
	RT_TASK_INFO taskInfo;

	rt_task_set_periodic(NULL, TM_NOW, 100);

	rt_task_inquire(NULL,&taskInfo);

	while(1){
		if (io_read(test) == 0)
		{
			io_write(test,0);
			rt_task_sleep(1000);
			io_write(test,1);
		}
	rt_task_wait_period(NULL);
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



int main()
{
	mlockall(MCL_CURRENT|MCL_FUTURE);
	pthread_t disturbanceThreads[DISTURBANCES];
	RT_TASK testTasks[TESTS];
	io_init();

	int i;
	char strBuff[sizeof(int)*4+1];

	for (i=0; i<DISTURBANCES; i++)
	{
		pthread_create(&disturbanceThreads[i], NULL, disturbance, NULL);
	}


	for (i=0; i<TESTS; i++)
	{
		sprintf(strBuff, "%d", i);
		rt_task_create(&testTasks[i], strBuff, 0, 50, T_CPU(1)|T_JOINABLE);
	}

	for (i=0; i<TESTS; i++)
	{
		rt_task_start(&testTasks[i], &testTask, i+1);
	}

	for (i=0; i<TESTS; i++)
	{
		rt_task_join(&testTasks[i]);
	}

	for (i=0; i<DISTURBANCES; i++)
	{
		pthread_join(disturbanceThreads[i], NULL);
	}
}