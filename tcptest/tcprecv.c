#include "head.h"

int shebeifd = 0;
yaohe a[8];


void translate(char *buff,int sockfd)
{
	char *p = NULL;
	char ch = 0;
	char tempbuff[128] = {0};
	int i = 0;
	int flag = 0;
	int fd = open("./log.txt",O_RDWR,O_CREAT);
	if(fd == -1)
	{
		perror("fail to open");
		return ;
	}

	p=strstr(buff,"#");
	if(p==NULL)
	{
		return;
	}
	flag = 0;
	ch = p[1];
	if(ch == '1')
	{
		shebeifd = sockfd;
		printf("shebei connencted\n");
	}
	else if(ch == 'A')
	{
		for(i = 0;i < 8;i++)
		{
			if(a[i].state == 0)
			{
				sprintf(tempbuff,"#D%d\n",i);
				send(shebeifd,tempbuff,strlen(tempbuff),0);
				send(sockfd,"ok",2,0);
				recv(shebeifd,tempbuff,128,0);
				printf("A DE esp8266:%s\n",tempbuff);
				printf("the mdicine has been placed %d the vcode is %c%c%c%c\n",i,p[2],p[3],p[4],p[5]);
				memset(tempbuff,0,sizeof(tempbuff));
				sprintf(tempbuff,"the mdicine has been placed %d,the verify code is %c%c%c%c,please take is away",i,p[2],p[3],p[4],p[5]);
				send(sockfd,tempbuff,strlen(tempbuff),0);
				strncpy(a[i].vcode,p+2,4);
				a[i].state = 1;
				break;
			}
		}
	}		
	else if(ch == 'B')
	{
		for(i = 0;i < 8;i++)
		{
			if(a[i].state == 1)
			{
				if(!strncmp(a[i].vcode,p+2,4))
				{
					sprintf(tempbuff,"#F%d\n",i);
					send(shebeifd,tempbuff,4,0);
					memset(tempbuff,0,sizeof(tempbuff));
					recv(shebeifd,tempbuff,sizeof(tempbuff),0);
					printf("shoudao esp8266:%s\n",tempbuff);
					printf("the medicine %d has been token\n",i);
					a[i].state = 0;
					memset(a[i].vcode,0,4);
					flag = 1;	
					break;
				}
			}
		}
		if(flag == 0)
		{
			send(shebeifd,"#H\n",3,0);
		}
	}
	else if(ch == 'C')
	{
		write(fd,p+2,6);
	}
	
	close(fd);
}

int Tcp_Init(const char *argv[])
{
	int sockfd = 0;
	int ret = 0;
	ssize_t nsize = 0;
	struct sockaddr_in tcpaddr;

	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0)
	{
		perror("faile to socket");
		return -1;
	}

	tcpaddr.sin_family = AF_INET;
	tcpaddr.sin_port = htons(atoi(argv[2]));
	tcpaddr.sin_addr.s_addr = inet_addr(argv[1]);
	ret = bind(sockfd,(struct sockaddr*)&tcpaddr,sizeof(tcpaddr));
	if(ret == -1)
	{
		perror("fail to bind");
		return -1;
	}

	ret = listen(sockfd,10);
	if(ret == -1)
	{
		perror("fail to listen");
		return -1;
	}

	return sockfd;

}

int main(char argc,const char *argv[])
{
	if(argc != 3)
	{
		fprintf(stderr,"toofew input\r\nplease input your ip and port\n");
		return -1;
	}

	if(strlen(argv[1])<strlen(argv[2]))
	{
		fprintf(stderr,"please follow \"ip port\"\n");
		return -1;
	}

	int confd = 0;
	int epfd = 0;
	int sockfd = 0;
	int nready = 0;
	int i = 0;
	int ret = 0;
	ssize_t nsize = 0;
	char tempbuff[1024] = {0};
	char buff[1024] = {0};
	struct epoll_event retenv[1024];
	struct epoll_event env;

	memset(a,0,sizeof(a));

	sockfd = Tcp_Init(argv);
	if(sockfd == -1)
	{
		perror("fail to Tcp_Init");
		return -1;
	}
	
	epfd = epoll_create(1024);
	if (-1 == epfd)
	{
		perror("fail to epoll_create");
		return -1;	
	}

	env.events = EPOLLIN;
	env.data.fd = sockfd;
	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &env);
	if (-1 == ret)
	{
		perror("fail to epoll_ctl");
		return -1;
	}



	while(1)
	{
		nready = epoll_wait(epfd, retenv, 1024, -1);
		if (-1 == nready)	
		{
			perror("fail to epoll_wait");
			return -1;
		}
		
		for (i = 0; i < nready; i++)			
		{
			if (retenv[i].data.fd == sockfd)
			{
				confd = accept(retenv[i].data.fd, NULL, NULL);
				if (-1 == confd)
				{
					perror("fail to accept");
					close(sockfd);
					epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, NULL);
					continue;
				}
				env.events = EPOLLIN;
				env.data.fd = confd;
				epoll_ctl(epfd, EPOLL_CTL_ADD, confd, &env);
				printf("a device connected\n");
			}
			else
			{
				memset(tempbuff,0,sizeof(tempbuff));
				nsize = recv(retenv[i].data.fd,tempbuff,sizeof(tempbuff),0);
				if (-1 == nsize)
				{
					perror("fail to recv");
					close(retenv[i].data.fd);
					epoll_ctl(epfd, EPOLL_CTL_DEL, retenv[i].data.fd, NULL);
					continue;
				}
				else if (0 == nsize)
				{
					printf("connecting break\n");
					close(retenv[i].data.fd);
					epoll_ctl(epfd, EPOLL_CTL_DEL, retenv[i].data.fd, NULL);
					continue;
				}		
				printf("recv: %s\n",tempbuff);
				translate(tempbuff,retenv[i].data.fd);		
			}
		}

	}
	
}
