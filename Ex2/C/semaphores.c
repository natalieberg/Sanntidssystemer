#include <semaphore.h>
#include <stdio.h>

sem_t sem3;

void accessResource(int threadNr){
	sem_wait(&sem3);
	printf("Thread nr: %d\n", threadNr);
	sleep(3);
	sem_post(&sem3);
}

int main(){

	sem_init(&sem3, 0, 3);

	pthread_t t1;
  	pthread_t t2;
	pthread_t t3;
  	pthread_t t4;
  	pthread_t t5;
  	pthread_create(&t1, NULL, accessResource, 1);
  	pthread_create(&t2, NULL, accessResource, 2);
  	pthread_create(&t3, NULL, accessResource, 3);
  	pthread_create(&t4, NULL, accessResource, 4);
  	pthread_create(&t5, NULL, accessResource, 5);
  	pthread_join(t1, NULL);
  	pthread_join(t2, NULL);
  	pthread_join(t3, NULL);
  	pthread_join(t4, NULL);
  	pthread_join(t5, NULL);


	return 0;
}