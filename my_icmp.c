#include "my_icmp.h"

u_int16_t checksum(unsigned short *addr,int len);

ICMP *icmp_init(void)
{
	ICMP *icmp;
	struct timeval timeout;

	icmp=(ICMP *)malloc(sizeof(ICMP));
	if(!icmp) return NULL;

	icmp->sockfd=socket(AF_INET,SOCK_RAW,IPPROTO_ICMP);
	icmp->lost=0;
	if(icmp->sockfd == -1)
	{
		free(icmp);
		return NULL;
	}
	icmp->id=getpid();
	memset(&icmp->icmp,0,sizeof(struct icmphdr));
	icmp->icmp.type=ICMP_ECHO;
	icmp->icmp.un.echo.id=htons(icmp->id);
	icmp->icmp.code=htons(0);
	icmp->avg=0.0;

	timeout.tv_sec=3;
	timeout.tv_usec=0;
	setsockopt(icmp->sockfd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));

	return icmp;
}

ssize_t icmp_send(ICMP *icmp,struct sockaddr *to)
{
	gettimeofday(&icmp->tv[0],NULL);

	return sendto(icmp->sockfd,&icmp->icmp,sizeof(struct icmphdr),
			0,to,sizeof(struct sockaddr));
}

bool icmp_exec(ICMP *icmp,char *host,int n)
{
	ssize_t r;
	int i,c;
	struct hostent *h;
	struct sockaddr_in to,from;
	struct 
	{
		struct iphdr ip;
		struct icmphdr icmp;
	}buf;
	socklen_t addrlen=sizeof(struct sockaddr);

	h=gethostbyname(host);
	if(!h)
	{
		herror(host);
		return false;
	}

	memset(&to,0,sizeof(to));
	to.sin_family=AF_INET;
	to.sin_addr=*(struct in_addr *)*h->h_addr_list;

	for(i=0,c=0;i < n;++i,sleep(1))
	{
		icmp->icmp.un.echo.sequence=htons(i+1);
		icmp->icmp.checksum=0;
		icmp->icmp.checksum=checksum((unsigned short *)&icmp->icmp,sizeof(struct icmphdr));

		r=icmp_send(icmp,(struct sockaddr *)&to);
		if(r <= 0)
		{
			herror("sendto");
			return false;
		}

		while(1)
		{
			memset(&from,0,sizeof(from));
			memset(&buf,0,sizeof(buf));
			r=recvfrom(icmp->sockfd,&buf,sizeof(buf),0,
					(struct sockaddr *)&from,&addrlen);
			gettimeofday(&icmp->tv[1],NULL);
			if(r <= 0)
			{
				if(h_errno == HOST_NOT_FOUND)
				{
					++icmp->lost;
					break;
				}

				continue;
			}

			if(memcmp(&from.sin_addr,&to.sin_addr,sizeof(to.sin_addr)) != 0)
				continue;

			if(buf.icmp.type == ICMP_ECHOREPLY)
			{
				if(ntohs(buf.icmp.un.echo.id) != icmp->id)
					continue;

				icmp->avg+=((1000000*(icmp->tv[1].tv_sec-icmp->tv[0].tv_sec)+icmp->tv[1].tv_usec-icmp->tv[0].tv_usec)/1000.0);
				++c;

				break;
			}
		}
	}

	if(c == 0)
		return false;
	icmp->avg/=c;
	return true;
}

float icmp_average(ICMP *icmp)
{
	return icmp->avg;
}

void icmp_free(ICMP *icmp)
{
	close(icmp->sockfd);
	free(icmp);
}

void icmp_reset(ICMP *icmp)
{
	icmp->avg=0.0;
	icmp->lost=0;
}

u_int16_t checksum(unsigned short *addr,int len)
{
	int nleft=len;
	int sum=0;
	unsigned short *w=addr;
	unsigned short answer=0;

	while(nleft > 1)
	{
		sum+=*w++;
		nleft-=2;
	}

	if(nleft == 1)
	{
		*(unsigned char *)&answer=*(unsigned char *)w;
		sum+=answer;
	}

	sum=(sum>>16)+(sum&0xffff);
	sum+=(sum>>16);

	return ~sum;
}
