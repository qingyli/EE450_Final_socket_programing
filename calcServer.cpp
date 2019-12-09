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
#include <fstream>
#include <math.h>
#define PORT 24935

using namespace std;

int main(){
    int client_fd;
    int Size, C, L, Vp;
	double Tt, Tp, Td;
	int len, sin_size;
	struct sockaddr_in ser_addr, cli_addr,trash;
	char message[1024] = {0};
    string buff;
    stringstream ss, ss_1;

	memset(&ser_addr,0,sizeof(ser_addr));

	ser_addr.sin_family=AF_INET;
	ser_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	ser_addr.sin_port=htons(22935);

    if((client_fd=socket(AF_INET,SOCK_DGRAM,0))<0)
	{
		perror("socket error");
		return 1;
	}
    cli_addr.sin_family=AF_INET;
	cli_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	cli_addr.sin_port=htons(PORT);

    if(bind(client_fd,(struct sockaddr *)&cli_addr,sizeof(cli_addr))<0)
	{
		perror("bind error\n");
		return 1;
	}

    cout << "The Calculation Server is up and running" << endl;
    while(1){
	recvfrom(client_fd, message, 1024, MSG_WAITALL, (struct sockaddr*)&trash, (socklen_t*)&sin_size);
    cout << "Receive request from Main Server." << endl;
	buff = message;
	//cout << buff << endl;
	//ss.str(buff);
	ss << buff;
	ss >> Size >> C >> L >> Vp;
	//cout << "Size: " << Size << " C: " << C << " L: " << L << " Vp: " << Vp << endl;
	Tt = 8*1000*Size/(double)C;
	Tp = 1000*L/(double)Vp;
	Td = Tp + Tt;

	Tt = roundf(Tt*100)/100;
	
	Tp = roundf(Tp*100)/100;
	
    Td = roundf(Td*100)/100;

	ss.str(string());
   	ss.clear();
	
	ss << Tt << " " << Tp << " " << Td;
	buff = ss.str();
	//cout << buff << endl;
	strcpy(message, buff.c_str());
	sendto(client_fd, message, 1024, 0, (struct sockaddr*)&ser_addr, sizeof(ser_addr));
	cout << "Send transmission delay " << Tt << "ms, propagation delay " << Tp << "ms and total delay " << Td << "ms" << endl;
	ss.str(string());
	ss.clear();
	memset(message,0,1024);
    }
	close(client_fd);
    return 0;
}
