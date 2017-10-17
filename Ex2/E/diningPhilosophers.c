#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

pthread_mutex_t fork1;
pthread_mutex_t fork2;


void *philosopher1(void *vargp){
	pthread_mutex_lock(&fork1);
	sleep(1);
	pthread_mutex_lock(&fork2);
	printf("Philosopher 1 is eating.\n");
	sleep(1);
	pthread_mutex_unlock(&fork1);
	pthread_mutex_unlock(&fork2);

}

void *philosopher2(void *vargp){
	pthread_mutex_lock(&fork2);
	sleep(1);
	pthread_mutex_lock(&fork1);
	printf("Philosopher 2 is eating.\n");
	sleep(1);
	pthread_mutex_unlock(&fork2);
	pthread_mutex_unlock(&fork1);

}


int main(){


	pthread_t t1;
  	pthread_t t2;
  	pthread_create(&t1, NULL, philosopher1, NULL);
  	pthread_create(&t2, NULL, philosopher2, NULL);
  	pthread_join(t1, NULL);
  	pthread_join(t2, NULL);


	return 0;
}