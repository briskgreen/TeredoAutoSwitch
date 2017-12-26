#ifndef _MY_TEREDO_H
#define _MY_TEREDO_H

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>

#define PORT 3544
#define SERVER_NUMS 10

void tt_test(int sockfd,const char *teredo_server_lists[],
		const char *list[]);

#endif
