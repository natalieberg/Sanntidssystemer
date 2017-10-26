#include "miniproject.h"
#include <sys/time.h>

pthread_mutex_t conn_mutex;
pthread_mutex_t y_mutex;
double y = 0;
struct udp_conn conn;
bool is_simulating = true;
bool signal_received = false;

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
	pthread_mutex_lock(&conn_mutex);
	udp_send(udp, startMsg, 6);
	pthread_mutex_unlock(&conn_mutex);
}

void stop_simulation(struct udp_conn *udp){
	char stopMsg[5] = "STOP";
	pthread_mutex_lock(&conn_mutex);
	udp_send(udp, stopMsg, 5);
	pthread_mutex_unlock(&conn_mutex);
}

void request_y(struct udp_conn *udp){
	char getMsg[4] = "GET";
	pthread_mutex_lock(&conn_mutex);
	udp_send(udp, getMsg, 4);
	pthread_mutex_unlock(&conn_mutex);
}


void set_u(struct udp_conn *udp, double u){
	//printf("Start set \n");
	char msgSetU[200] = "SET:";
	char uChar[100];
	//printf("%f\n", u);
	sprintf(uChar, "%f", u);
	//printf("%s\n", uChar);
	//printf("%s\n", msgSetU);
	strcat(msgSetU, uChar);
	//printf("%s\n", msgSetU);
	pthread_mutex_lock(&conn_mutex);
	udp_send(udp, msgSetU, 200);
	pthread_mutex_unlock(&conn_mutex);
	//printf("Stop set \n");
}

bool compare_times(struct timespec *end, struct timespec *period){
	return end->tv_sec > period->tv_sec || (end->tv_sec == period->tv_sec && end->tv_nsec > period->tv_nsec);
	// printf("%lld.%.9ld \n", (long long)end->tv_sec, end->tv_nsec);
	// printf("%lld.%.9ld \n", (long long)period->tv_sec, period->tv_nsec);
	// if (end->tv_sec > period->tv_sec) {
	// 	printf("true \n");
	// 	return true;
	// } else if (end->tv_sec < period->tv_sec) {
	// 	printf("false \n");
	// 	return false;
	// } else {
	// 		if(end->tv_nsec > period->tv_nsec ) {
	// 			printf("true \n");
	// 			return true;
	// 		} else if (end->tv_nsec <= period->tv_nsec )
	// 				printf("false \n");
	// 				return false;
	// }
}

void *controller(void *argp){
	double u = 0;
	double error;
	double r = 1;
	double integral = 0;
	double deltat = 0.001;
	double Kp = 10;
	double Ki = 800;

	struct timespec period;
	struct timespec end;

	clock_gettime(CLOCK_REALTIME, &end);
	clock_gettime(CLOCK_REALTIME, &period);
	timespec_add_us(&end, 0.5*1000000);

		while (compare_times(&end, &period)) {
			//printf("start while \n");
			request_y(&conn);
			pthread_mutex_lock(&y_mutex);
			printf("mutex lock in controller \n");
			error = r-y;
			pthread_mutex_unlock(&y_mutex);
			printf("mutex unlock in controller \n");
			integral = integral+(error*deltat);
			u = Kp*error+Ki*integral;
			set_u(&conn, u);

			timespec_add_us(&period, 0.001*1000000);
			clock_nanosleep(&period);
			//printf("end while \n");
		}

		printf("simulating = false \n");
		is_simulating = false;
}

void *udp_listen(void *argp){
	while (is_simulating) {
		//printf("start udp while \n");
		int len;
		char msgRecv[100];
		char check[8];
		char *yChar;
		//printf("before udp recv \n");
		len = udp_receive(&conn, msgRecv, 100);
		//printf("Msg recv: %s\n", msgRecv);
		memset(check, '\0', sizeof(check));
		strncpy(check, msgRecv, 6);
		//printf("check: %s\n", check);
		if ((strcmp(check, "GET_AC") == 0)){
			strtok_r(msgRecv, ":", &yChar);
			//printf("%s\n", yChar);
			pthread_mutex_lock(&y_mutex);
			printf("mutex lock in udp \n");
			y = atof(yChar);
			pthread_mutex_unlock(&y_mutex);
			printf("mutex unlock in udp \n");
			//printf("%f\n", y);
			//printf("Stop recv \n");
		} else if (((strcmp(check, "SIGNAL") == 0))) {
			//signal_received = true;
		}
		//printf("end udp while \n");
		usleep(0.1);
	}
}

void *signal_respond(void *argp){
	char ackMsg[11] = "SIGNAL_ACK";
	while (is_simulating) {
		if (signal_received) {
			pthread_mutex_lock(&conn_mutex);
			udp_send(&conn, ackMsg, 11);
			pthread_mutex_unlock(&conn_mutex);
			signal_received = false;
		}
	}
}


int main() {

	char IPAddr[12] = "192.168.0.1";

	pthread_t udp_listen_thread;
	pthread_t controller_thread;
	pthread_t ack_thread;

	udp_init_client(&conn, 9999, IPAddr);
	start_simulation(&conn);

	pthread_create(&udp_listen_thread, NULL, udp_listen, NULL);
	pthread_create(&controller_thread, NULL, controller, NULL);
	//pthread_create(&ack_thread, NULL, signal_respond, NULL);

	pthread_join(udp_listen_thread, NULL);
	pthread_join(controller_thread, NULL);
	pthread_join(ack_thread, NULL);

	stop_simulation(&conn);
	udp_close(&conn);

	return 0;
}
