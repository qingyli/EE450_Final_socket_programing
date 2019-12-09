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
#define monPORT 25935
#define udpPORT 22935
#define dbPORT 23935
#define calcPORT 24935

using namespace std;

int main(){
    int ser_cli_fd, client_fd;
    int ser_mon_fd, moniter_fd;
    int ser_udp_fd;
    int mon_size, cli_size, sin_size;
    struct sockaddr_in ser_cli_addr, cli_addr;
    struct sockaddr_in ser_mon_addr, mon_addr;
    struct sockaddr_in ser_addr, db_addr, calc_addr, trash;
    char buff[1024] = {0};
    string mon_msg ,msg;
    stringstream ss;

    int ID, size, C, L, Vp;
    double Tt, Td, Tp;
    
    //UDP
    ser_udp_fd = socket(AF_INET,SOCK_DGRAM,0); 
    if (ser_udp_fd == -1) { 
        perror("socket error\n"); 
        return 1; 
    }
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    ser_addr.sin_port = htons(udpPORT); 
    db_addr.sin_family=AF_INET;
	db_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	db_addr.sin_port=htons(dbPORT);
    calc_addr.sin_family=AF_INET;
	calc_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	calc_addr.sin_port=htons(calcPORT);

    if(bind(ser_udp_fd,(struct sockaddr *)&ser_addr,sizeof(ser_addr))<0)
	{
		perror("bind udp error\n");
		return 1;
	}

    //client
    ser_cli_fd = socket(AF_INET,SOCK_STREAM,0);
    if (ser_cli_fd == -1) { 
        perror("socket client error\n"); 
        return 1; 
    }
    ser_cli_addr.sin_family = AF_INET;
    ser_cli_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    ser_cli_addr.sin_port = htons(cliPORT); 
    if(bind(ser_cli_fd,(struct sockaddr *)&ser_cli_addr,sizeof ser_cli_addr)<0)
	{
		perror("bind client error\n");
		return 1;
	}
    //
        
    cout << "The Main Server is up to running" << endl;
    
    //moniter
    ser_mon_fd = socket(AF_INET,SOCK_STREAM,0); 
    if (ser_mon_fd == -1) { 
        perror("socket moniter error\n"); 
        return 1; 
    }
    ser_mon_addr.sin_family = AF_INET;
    ser_mon_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    ser_mon_addr.sin_port = htons(monPORT); 
    if(bind(ser_mon_fd,(struct sockaddr *)&ser_mon_addr,sizeof ser_mon_addr)<0)
	{
		perror("bind moniter error\n");
		return 1;
	}
    if(listen(ser_mon_fd,5)<0)
	{
        perror("listen moniter error");
	    return 1;
    };
    mon_size = sizeof(mon_addr);
    if((moniter_fd=accept(ser_mon_fd,(struct sockaddr *)&mon_addr, (socklen_t*)&mon_size))<0)
    {
		perror("accept moniter error");
	    return 1;
    }

    //cout << "The Main Server is up to running" << endl;

    while(1){
        if(listen(ser_cli_fd,5)<0)
        {
            perror("listen client error");
            return 1;
        }
        cli_size = sizeof(cli_addr);
        if((client_fd=accept(ser_cli_fd,(struct sockaddr *)&cli_addr, (socklen_t*)&cli_size))<0)
        {
		    perror("accept client error");
	        return 1;
        }
        recv(client_fd,buff,1024,0);
        ID = atoi(buff);
        send(client_fd,buff,1024,0);
        memset(buff, 0, 1024);
        recv(client_fd,buff,1024,0);
        //send(client_fd,buff,1024,0);
        size = atoi(buff);

        memset(buff, 0, 1024);

        ss << "Main server receives Link " << ID << " and file size " << size << "MB from client.";
        mon_msg = ss.str();
        ss.str(string());
        ss.clear(); 
        strcpy(buff, mon_msg.c_str());
        send(moniter_fd,buff,1024,0);
        cout << "Receive Link " << ID << ", file size " << size << "MB." << endl;
        memset(buff, 0, 1024);



        ss << ID;
        msg = ss.str();
        strcpy(buff, msg.c_str());
        ss.str(string());
        ss.clear(); 
        sendto(ser_udp_fd, buff, 1024, MSG_CONFIRM, (struct sockaddr*)&db_addr, sizeof(db_addr));
        memset(buff, 0, 1024);
        cout << "Send Link " << ID << " to database server." << endl;
        ss << "Main server sends Link " << ID << " to database server.";
        mon_msg = ss.str();
        strcpy(buff, mon_msg.c_str());
        send(moniter_fd,buff,1024,0);
        ss.str(string());
        ss.clear(); 
        memset(buff, 0, 1024);

        recvfrom(ser_udp_fd, buff, 1024, MSG_WAITALL, (struct sockaddr*)&trash, (socklen_t*)&sin_size);
        msg = buff;
        if(msg == "Recieve no match found"){
            cout << buff << endl;
            mon_msg = "Main server receives information from calculation server: no match found";
            strcpy(buff, mon_msg.c_str());
            send(moniter_fd,buff,1024,0);
            memset(buff,0,1024);
            msg = "No match found";
            strcpy(buff, msg.c_str());
            send(client_fd, buff, 1024, 0);
        }
        else{
            ss.str(buff);
            ss >> C >> L >> Vp;
            cout << "Receive link capacity " << C << "Mbps, link length " << L << "km, propagation velocity " << Vp << "km/s." << endl;
            ss.str(string());
            ss.clear();
            ss << "Main server receives information from database server: link capacity " << C << "Mps, link length " << L << "km, and propagation velocity " << Vp << "km/s.";
            mon_msg = ss.str();
            strcpy(buff, mon_msg.c_str());
            send(moniter_fd,buff,1024,0);
            ss.str(string());
            ss.clear();
            memset(buff,0,1024);
        

            cout << "Send information to calculation server." << endl;
            mon_msg = "Main Server sends information to calculation server.";
            strcpy(buff, mon_msg.c_str());
            send(moniter_fd,buff,1024,0);
            //recvfrom(server_fd, message, 1024, MSG_WAITALL, (struct sockaddr*)&trash, (socklen_t*)&sin_size);
            ss << size << " " << C << " " << L << " " << Vp;
            msg = ss.str();
            ss.str(string());
            ss.clear();
            //cout << msg << endl;
            strcpy(buff, msg.c_str());
            sendto(ser_udp_fd, buff, 1024, MSG_CONFIRM, (struct sockaddr*)&calc_addr, sin_size);
            recvfrom(ser_udp_fd, buff, 1024, MSG_WAITALL, (struct sockaddr*)&trash, (socklen_t*)&sin_size);
            //cout << "message" << message << endl;
            msg = buff;
            send(client_fd,buff,1024,0);
            ss.str(msg);
            ss >> Tt;
            ss >> Tp;
            ss >> Td;
            ss.str(string());
            ss.clear();
            cout << "Receive transmission delay " << Tt << "ms, propagation delay " << Tp << "ms and total delay " << Td << "ms" << endl;
            mon_msg = "Main Server receives information to calculation server.";
            strcpy(buff, mon_msg.c_str());
            send(moniter_fd,buff,1024,0);

            ss << "Main Server sends information to client: transmission delay " << Tt << "ms, propagation delay " << Tp << "ms and total delay " << Td << "ms.";
            mon_msg = ss.str();
            
            strcpy(buff, mon_msg.c_str());
            send(moniter_fd,buff,1024,0);
            memset(buff,0,1024);


        }
        ss.str(string());
        ss.clear();
        memset(buff,0,1024);
    }

    //never reached
    return 0;
}