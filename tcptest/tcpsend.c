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
	ret = connect(sockfd,(struct sockaddr*)&tcpaddr,sizeof(tcpaddr));
	if(ret == -1)
	{
		perror("fail to connect");
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

	int sockfd = 0;
	ssize_t nsize = 0;
	unsigned char tempbuff[1024] = {0};
	
	sockfd = Tcp_Init(argv);
	if(sockfd == -1)
	{
		perror("fail to Tcp_Init");
		return -1;
	}
	

	while(1)
	{
		fgets(tempbuff,32,stdin);
		send(sockfd,tempbuff,strlen(tempbuff),0);
		memset(tempbuff,0,sizeof(tempbuff));
		recv(sockfd,tempbuff,sizeof(tempbuff),0);
		printf("orangepi zero %s\n",tempbuff);
		memset(tempbuff,0,sizeof(tempbuff));
		
	}
	
}
