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
#include <sstream>

#define cliPORT 21935

using namespace std;

int main(int argc, char *argv[])
{
	int client_fd;
	int ID,size;
    ID = atoi(argv[1]);
    size = atoi(argv[2]);
    string name;
	string msg;
	struct sockaddr_in ser_addr;
	char buff[1024] = {0};
	stringstream ss;
	double Tt, Td, Tp;

	memset(&ser_addr,0,sizeof(ser_addr));

	ser_addr.sin_family=AF_INET;
	ser_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	ser_addr.sin_port=htons(cliPORT);
	
	if((client_fd=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		perror("socket error");
		return 1;
	}
	
	if(connect(client_fd,(struct sockaddr *)&ser_addr,sizeof(ser_addr))<0)
	{
		perror("connect error");
		return 1;
	}

	cout << "The client is up and running" << endl;
    cout << "Send Link " << ID << " and file size " << size << "MB to main server." << endl;
    strcpy(buff, argv[1]);
    send(client_fd,buff,1024,0);
    recv(client_fd,buff,1024,0);
    memset(buff,0,1024);
    send(client_fd, argv[2], 10,0);

	recv(client_fd, buff, 1024,0);
	msg = buff;
	//cout << msg << endl;
	if(msg == "No match found"){
		cout << buff << endl;
	}
	else{
		ss.str(buff);
		ss >> Tt >> Tp >> Td;
		cout << "Receive transmission delay " << Tt << "ms, propagation delay " << Tp << "ms, total delay " << Td << "ms." << endl;
	}

	close(client_fd);
    
	return 0;
}