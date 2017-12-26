#include "my_teredo.h"
#include "my_icmp.h"
#include "simple_conf.h"

#define ICMP_TIMES 10
#define MIREDO_CONF_PATH "/etc/miredo.conf"

typedef struct
{
	char *h;
	int lost;
	float avg;
}DATA;

int cmp(const void *a,const void *b)
{
	DATA *x=(DATA *)a;
	DATA *y=(DATA *)b;

	if(x->lost <= y->lost)
		return 1;

	if(x->avg >= y->avg)
		return 1;

	return -1;
}

void save_to_file(SMC *m,char *h)
{
	FILE *fp;
	int i;

	if(!(fp=fopen(MIREDO_CONF_PATH,"wb")))
	{
		perror("fopen");
		return;
	}

	for(i=0;i < m->n;++i)
	{
		if(strcmp(m->h[i].key,"ServerAddress") == 0)
			fprintf(fp,"%s\t%s\n",m->h[i].key,h);
		else
			fprintf(fp,"%s\t%s\n",m->h[i].key,m->h[i].value);
	}

	fclose(fp);
}

void modify_conf(DATA *data)
{
	SMC m;
	char *s;

	miredo_conf_init(&m);
	if(!miredo_conf_load(&m,MIREDO_CONF_PATH))
	{
		printf("Error!\n");
		return;
	}

	s=miredo_conf_search(&m,"ServerAddress");
	if(s)
	{
		if(strcmp(s,data->h) == 0)
			return;

		save_to_file(&m,data->h);
	}
}

int main(int argc,char *argv[])
{
	int sockfd;
	const char *teredo_server_lists[]=
	{
		"teredo.ipv6.microsoft.com",
		"teredo.remlab.net",
		"teredo2.remlab.net",
		"debian-miredo.progsoc.org",
		"teredo.ginzado.ne.jp",
		"teredo.iks-jena.de",
		"win10.ipv6.microsoft.com",
		"teredo.managemydedi.com",
		"teredo.trex.fi",
		"win1710.ipv6.microsoft.com",
		NULL
	};
	const char *lists[SERVER_NUMS];
	struct timeval ti;
	const char **p=(const char **)lists;
	ICMP *icmp;
	DATA save_lists[SERVER_NUMS];
	int i;

	sockfd=socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd == -1)
	{
		herror("socket");
		return -1;
	}
	ti.tv_sec=3;
	ti.tv_usec=0;
	setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,&ti,sizeof(ti));

	printf("Server Test\n");
	printf("===============================================\n");
	tt_test(sockfd,teredo_server_lists,lists);
	close(sockfd);

	icmp=icmp_init();
	if(!icmp)
		return -1;

	printf("\nPing Test");
	printf("\n===============================================\n");
	i=0;
	while(*p)
	{
		printf("\t%s . . .",*p);
		if(icmp_exec(icmp,(char *)*p,ICMP_TIMES))
		{
			printf(" %.2f ms\n",icmp_average(icmp));
			if(icmp->lost == ICMP_TIMES)
			{
				++p;
				continue;
			}

			save_lists[i].h=(char *)*p;
			save_lists[i].lost=icmp->lost;
			save_lists[i].avg=icmp_average(icmp);
			++i;
		}
		else
			printf(" Error\n");

		icmp_reset(icmp);

		++p;
	}
	icmp_free(icmp);

	if(i != 0)
	{
		qsort(save_lists,i,sizeof(DATA),cmp);
		modify_conf(&save_lists[0]);
		system("systemctl restart miredo.service");
	}

	return 0;
}
