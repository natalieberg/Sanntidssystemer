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
#include <native/sem.h>
#include <rtdk.h>

#define TASKS 3

RT_SEM syncSem;
RT_SEM semSem;

void busy_wait_ms(unsigned long delay)
{
	unsigned long count = 0;
	while (count <= delay*10)
	{
		rt_timer_spin(1000*100);
	count++;
	}
} 

int rt_task_sleep_ms(unsigned long delay)
{
	return rt_task_sleep(1000*1000*delay);
}

void print_pri(RT_TASK *task, char *s)
{
	struct rt_task_info temp;
	rt_task_inquire(task, &temp);
	rt_printf("b:%i c:%i ", temp.bprio, temp.cprio);
	rt_printf(s);
}


void syncTask(void *argp)
{	
	printf("Hey from sync task \n");

	RT_TASK_INFO taskInfo;

	rt_task_inquire(NULL,&taskInfo);

	rt_task_sleep_ms(100);

	rt_sem_broadcast(&syncSem);
}

void testTask(int task)
{	
	rt_sem_p(&syncSem, TM_INFINITE);

	RT_TASK_INFO taskInfo;

	rt_task_inquire(NULL,&taskInfo);

	switch(task)
	{
		case 1:
			rt_printf("%d start \n",task);
			rt_sem_p(&semSem, TM_INFINITE);
			rt_printf("%d running \n",task);
			busy_wait_ms(300);
			rt_printf("%d end \n",task);
			rt_sem_v(&semSem);
			break;
		case 2:
			rt_printf("%d start \n",task);
			rt_task_sleep_ms(100);
			rt_printf("%d running \n",task);
			busy_wait_ms(500);
			rt_printf("%d end \n",task);
			break;
		case 3:
			rt_printf("%d start \n",task);
			rt_task_sleep_ms(200);
			rt_sem_p(&semSem, TM_INFINITE);
			rt_printf("%d running \n",task);
			busy_wait_ms(200);
			rt_printf("%d end \n",task);
			rt_sem_v(&semSem);
			break;
	}
}

int main()
{
	mlockall(MCL_CURRENT|MCL_FUTURE);
	RT_TASK testTasks[TASKS];
	RT_TASK synchronize;
	rt_print_auto_init(1);
	//io_init();

	rt_sem_create(&syncSem, "sync", 0, S_PRIO);
	rt_sem_create(&semSem, "resource", 1, S_PRIO);

	int i;
	char strBuff[sizeof(int)*4+1];

	for (i=0; i<TASKS; i++)
	{
		sprintf(strBuff, "%d", i);
		rt_task_create(&testTasks[i], strBuff, 0, i+1, T_CPU(1)|T_JOINABLE);
	}

	rt_task_create(&synchronize, "SyncTask", 0, 50, T_CPU(1)|T_JOINABLE);
	

	for (i=0; i<TASKS; i++)
	{
		rt_task_start(&testTasks[i], &testTask, i+1);
	}
	rt_task_sleep_ms(10);

	rt_task_start(&synchronize, &syncTask, NULL);

	for (i=0; i<TASKS; i++)
	{
		rt_task_join(&testTasks[i]);
	}

	rt_task_join(&synchronize);

	rt_sem_delete(&syncSem);
	rt_sem_delete(&semSem);
}