#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>

#include "miniproject.h"

int udp_init_client(struct udp_conn *udp, int port, char *ip)
{
	struct hostent *host;

	if ((host = gethostbyname(ip)) == NULL) return -1;

	udp->client_len = sizeof(udp->client);
	// define servers
	memset((char *)&(udp->server), 0, sizeof(udp->server));
	udp->server.sin_family = AF_INET;
	udp->server.sin_port = htons(port);
	bcopy((char *)host->h_addr, (char *)&(udp->server).sin_addr.s_addr, host->h_length);

	// open socket
	if ((udp->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) return udp->sock;

	return 0;
}

int udp_send(struct udp_conn *udp, char *buf, int len)
{
	return sendto(udp->sock, buf, len, 0, (struct sockaddr *)&(udp->server), sizeof(udp->server));
}

int udp_receive(struct udp_conn *udp, char *buf, int len)
{
	int res = recvfrom(udp->sock, buf, len, 0, (struct sockaddr *)&(udp->client), &(udp->client_len));

	return res;
}

void udp_close(struct udp_conn *udp)
{
	close(udp->sock);
	return;
}

int clock_nanosleep(struct timespec *next)
{
	struct timespec now;
	struct timespec sleep;

	// get current time
	clock_gettime(CLOCK_REALTIME, &now);

	// find the time the function should sleep
	sleep.tv_sec = next->tv_sec - now.tv_sec;
	sleep.tv_nsec = next->tv_nsec - now.tv_nsec;

	// if the nanosecon is below zero, decrement the seconds
	if (sleep.tv_nsec < 0)
	{
		sleep.tv_nsec += 1000000000;
		sleep.tv_sec -= 1;
	}

	// sleep
	nanosleep(&sleep, NULL);

	return 0;
}

void timespec_add_us(struct timespec *t, long us)
{
	// add microseconds to timespecs nanosecond counter
	t->tv_nsec += us*1000;

	// if wrapping nanosecond counter, increment second counter
	if (t->tv_nsec > 1000000000)
	{
		t->tv_nsec -= 1000000000;
		t->tv_sec += 1;
	}
}

void start_simulation(struct udp_conn *udp){
	char startMsg[6] = "START";
	udp_send(udp, startMsg, 6);
}

void stop_simulation(struct udp_conn *udp){
	char stopMsg[5] = "STOP";
	udp_send(udp, stopMsg, 5);
}

void request_y(struct udp_conn *udp){
	char getMsg[4] = "GET";
	udp_send(udp, getMsg, 4);
}

double recv_y(struct udp_conn *udp){
	printf("Start recv \n");
	int len;
	char msgRecv[100];
	char check[8];
	char *yChar;
	double y;
	len = udp_receive(udp, msgRecv, 100);
	printf("Msg recv: %s\n", msgRecv);
	strncpy(check, msgRecv, 7);
	if ((strcmp(check, "GET_ACK") == 0)){
		strtok_r(msgRecv, ":", &yChar);
		//printf("%s\n", yChar);
		y = atof(yChar);
		//printf("%f\n", y);
		printf("Stop recv \n");
		return y;
	} else {
		return 0;
	}
}

void set_u(struct udp_conn *udp, double u){
	printf("Start set \n");
	char msgSetU[200] = "SET:";
	char uChar[100];
	//printf("%f\n", u);
	sprintf(uChar, "%f", u);
	//printf("%s\n", uChar);
	//printf("%s\n", msgSetU);
	strcat(msgSetU, uChar);
	printf("%s\n", msgSetU);
	udp_send(udp, msgSetU, 200);
	printf("Stop set \n");


}

int main() {

	double y;
	double u = 0;
	double error;
	double r = 1;
	double integral = 0;
	double deltat = 0.001;
	double Kp = 10;
	double Ki = 800;
	struct udp_conn conn1;
	struct timespec period;
	struct timespec end;
	char IPAddr[12] = "192.168.0.1";

	clock_gettime(CLOCK_REALTIME, &period);
	clock_gettime(CLOCK_REALTIME, &end);

	timespec_add_us(&end, 0.5*1000000);

	udp_init_client(&conn1, 9999, IPAddr);
	start_simulation(&conn1);

	while (/*COMPARE STUPID TIMES*/) {
		request_y(&conn1);
		 y = recv_y(&conn1);
		 printf(" y = %f\n", y);
		 error = r-y;
		 integral = integral+(error*deltat);
		 u = Kp*error+Ki*integral;
		 printf(" u = %f\n", u);
		 set_u(&conn1, u);
		 timespec_add_us(&period, 0.001*1000000);
		 clock_nanosleep(&period);
	}

	stop_simulation(&conn1);
	udp_close(&conn1);

	return 0;
}
