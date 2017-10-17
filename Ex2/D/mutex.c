#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

int running = 1;
int var1 = 0;
int var2 = 0;

pthread_mutex_t lock;

void *threadNr1(void *vargp){
	while (running == 1) {
		pthread_mutex_lock(&lock);
		var1++;
		var2 = var1;
		pthread_mutex_unlock(&lock);
	}

}

void *threadNr2(void *vargp){
	int i;
	for (i = 0; i < 20; i++){
		pthread_mutex_lock(&lock);
		printf("Number 1 is %d, number 2 is %d\n", var1, var2 );
		pthread_mutex_unlock(&lock);
		usleep(100000);
	}
	running = 0;
}

int main(){


	pthread_t t1;
  	pthread_t t2;
  	pthread_create(&t1, NULL, threadNr1, NULL);
  	pthread_create(&t2, NULL, threadNr2, NULL);
  	pthread_join(t1, NULL);
  	pthread_join(t2, NULL);

	return 0;
}