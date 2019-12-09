#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <iostream>

#define monPORT 25935

using namespace std;

int main(){
    int moniter_fd;
	struct sockaddr_in ser_addr;
	char buf[1024] = {0};

	memset(&ser_addr,0,sizeof(ser_addr));

	ser_addr.sin_family=AF_INET;
	ser_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	ser_addr.sin_port=htons(monPORT);
	
	if((moniter_fd=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		perror("socket error");
		return 1;
	}
	
	if(connect(moniter_fd,(struct sockaddr *)&ser_addr,sizeof(ser_addr))<0)
	{
		perror("connect error");
		return 1;
	}

	cout << "The Moniter is up and running." << endl;
    while(1){
        recv(moniter_fd,buf,1024,0);
        cout << buf << endl;
		memset(buf,0,1024);
    }

    

    //never reached
    close(moniter_fd);
    return 0;
}