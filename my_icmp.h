#ifndef _MY_ICMP_H
#define _MY_ICMP_H

#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <stdbool.h>

typedef struct 
{
	int sockfd;
	struct icmphdr icmp;
	int id;
	struct timeval tv[2];
	int lost;
	float avg;
}ICMP;

ICMP *icmp_init(void);
ssize_t icmp_send(ICMP *icmp,struct sockaddr *to);
bool icmp_exec(ICMP *icmp,char *host,int n);
float icmp_average(ICMP *icmp);
void icmp_free(ICMP *icmp);
void icmp_reset(ICMP *icmp);

#endif
