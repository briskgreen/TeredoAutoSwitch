#include "my_teredo.h"

void tt_test(int sockfd,const char *teredo_server_lists[],
		const char *list[])
{
	struct hostent *h;
	struct sockaddr_in to;
	char *buf="aaa";
	ssize_t n;
	const char **p=(const char **)teredo_server_lists;
	const char **s=(const char **)list;

	setbuf(stdout,NULL);
	while(*p)
	{
		printf("\t%s . . .",*p);
		h=gethostbyname(*p);
		if(!h)
		{
			printf(" NO\n");
			++p;
			continue;
		}

		memset(&to,0,sizeof(to));
		to.sin_family=AF_INET;
		to.sin_port=htons(PORT);
		to.sin_addr=*(struct in_addr *)*h->h_addr_list;

		n=sendto(sockfd,buf,sizeof(char)*3,0,
				(struct sockaddr *)&to,sizeof(to));
		if(n != 3)
			printf(" NO\n");
		else
		{
			*s=*p;
			++s;
			printf(" OK\n");
		}

		++p;
	}

	*s=NULL;
}
