#ifndef MINIPROJECT_H_
#define MINIPROJECT_H_

#include <arpa/inet.h>
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
#include <pthread.h>
#include <stdbool.h>

// typedef
typedef void (*sighandler_t)(int);

// structs that store the information needed for an udp connection
struct udp_conn{
	int sock;
	struct sockaddr_in server;
	struct sockaddr_in client;
	socklen_t client_len;
};

// initialize the struct and connect to a udp server on the given port and ip
int udp_init_client(struct udp_conn *udp, int port, char *ip);

// function for sending a string over an udp connection
int udp_send(struct udp_conn *udp, char *buf, int len);

// function for receiving a string over an udp connection
int udp_receive(struct udp_conn *udp, char *buf, int len);

// function for closing a connection
void udp_close(struct udp_conn *udp);

// function replacing clock_nanosleep
// DO NOT use for periods over 500 ms
int clock_nanosleep(struct timespec *next);

void timespec_add_us(struct timespec *t, long us);

void start_simulation(struct udp_conn *udp);
void stop_simulation(struct udp_conn *udp);
void request_y(struct udp_conn *udp);
double recv_y(struct udp_conn *udp);
void set_u(struct udp_conn *udp, double u);
void *controller(void *argp);
void *udp_listen(void *argp);
void *signal_respond(void *argp);

#endif /* MINIPROJECT_H_ */
