#ifndef __HEAD_H__
#define __HEAD_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/ip.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/epoll.h>

typedef struct _yaohe{
	char vcode[5];
	char state;
} yaohe;



#endif
