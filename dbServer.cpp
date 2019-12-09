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

#define udpPORT 22935
#define dbPORT 23935

using namespace std;

int main(){
    int client_fd;
    int ID, C, L, Vp;
    int sin_size;
	struct sockaddr_in ser_addr, cli_addr,trash_addr;
	char buff[1024] = {0};
    string temp, trash;
    stringstream ss;

	memset(&ser_addr,0,sizeof(ser_addr));

	ser_addr.sin_family=AF_INET;
	ser_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	ser_addr.sin_port=htons(udpPORT);

    if((client_fd=socket(AF_INET,SOCK_DGRAM,0))<0)
	{
		perror("socket error");
		return 1;
	}
    cli_addr.sin_family=AF_INET;
	cli_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	cli_addr.sin_port=htons(dbPORT);

    if(bind(client_fd,(struct sockaddr *)&cli_addr,sizeof(cli_addr))<0)
	{
		perror("bind error\n");
		return 1;
	}

    ifstream input;
    int buf;
    
    


    cout << "The Database Server is up and running" << endl;
    while(1){
        input.open("database.txt");
        temp = "Ini";

        recvfrom(client_fd, buff, 1024, MSG_WAITALL, (struct sockaddr*)&trash_addr, (socklen_t*)&sin_size);
        cout << "Receive request from Main Server." << endl;
    
        ID = atoi(buff);
        //cout << "ID: " << ID << endl;

        while(input >> buf){
            //cout << buf << endl;
            if(buf == ID)
            {
                input >> C >> L >> Vp;
                ss << C << " " << L << " " << Vp;
                temp = ss.str();
                break;
            }
            else
            {
                getline(input,trash);
            }
        }
    
        if(temp == "Ini")
        {
            cout << "No match found" << endl;
            temp = "Recieve no match found";
            strcpy(buff, temp.c_str());
        }
        else
        {
            cout << "Send link " << ID << ", capacity " << C << "Mbps, link length" << L << "km, propagation velocity " << Vp << "km/s." << endl;
 
            strcpy(buff, temp.c_str());
        }

        sendto(client_fd, buff, 1024, 0, (struct sockaddr*)&ser_addr, sin_size);
        ss.str(string());
        ss.clear(); 
        memset(buff,0,1024);
        input.close();
    }
    close(client_fd);
    return 0;
}
