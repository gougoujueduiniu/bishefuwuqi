#include "head.h"

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
	int sockfd = 0;
	ssize_t nsize = 0;
	char tempbuff[1024] = {0};
	char buff[1024] = {0};
	sockfd = Tcp_Init(argv);
	if(sockfd == -1)
	{
		perror("fail to Tcp_Init");
		return -1;
	}
	
	confd = accept(sockfd,NULL,NULL);
	printf("a device is connect\n");

	while(1)
	{
		nsize = recv(confd,tempbuff,sizeof(tempbuff),0);	
		if(nsize == -1)
		{
			perror("fail to recv");
			return -1;
		}
		

		sprintf(buff,"orangepi service %s",tempbuff);

		send(confd,buff,strlen(buff),0);
		memset(tempbuff,0,sizeof(tempbuff));
	}
	
}
