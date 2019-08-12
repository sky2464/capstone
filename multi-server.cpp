//Example code: A simple server side code, which echos back the received message. 
//Handle multiple socket connections with select and fd_set on Linux 
#include <stdio.h> 
#include <string.h> //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h> //close 
#include <arpa/inet.h> //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 

#include <sstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "jana-m.h"

using namespace std;
	
#define TRUE 1 
#define FALSE 0 
#define PORT 8888 

/*string timed_listen(int sck){
        time_t ts;
        time(&ts);
        char bfr[1024] = {0};
        string msg = "null";
        double sec;
        while(string(bfr).length() == 0 && sec < 11){
                time_t ct = time(NULL);
                sec = difftime(ct, ts);
                bfr[1024] = {0};
                int valread = read(sck, bfr, 1024);
        }
        if(sec < 11){msg = string(bfr);}
        return msg;
}*/


int main(int argc , char *argv[]) 
{ 
	int opt = TRUE; 
	int master_socket , addrlen , new_socket , client_socket[30] , 
		max_clients = 30 , activity, i , valread , sd; 
	int max_sd; 
	struct sockaddr_in address; 
		
	char buffer[1025]; //data buffer of 1K 
		
	//set of socket descriptors 
	fd_set readfds; 
		
	//a message 
	string ms = "ECHO Daemon v1.0 \r\n"; 
	const char *message = ms.c_str();
	
	//initialise all client_socket[] to 0 so not checked 
	for (i = 0; i < max_clients; i++) 
	{ 
		client_socket[i] = 0; 
	} 
		
	//create a master socket 
	if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	//set master socket to allow multiple connections , 
	//this is just a good habit, it will work without this 
	if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, 
		sizeof(opt)) < 0 ) 
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 
	
	//type of socket created 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORT ); 
		
	//bind the socket to localhost port 8888 
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	printf("Listener on port %d \n", PORT); 
		
	//try to specify maximum of 3 pending connections for the master socket 
	if (listen(master_socket, 3) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 
		
	//accept the incoming connection 
	addrlen = sizeof(address); 
	puts("Waiting for connections ..."); 
		
	string msg;//Full message from client, for memory simulation cap at 10
	int msg_sz = 0;
	int srt_bt = 0;
	int ky;
	vector<int> errlog;
	errlog.reserve(max_clients);
	errlog = {0};

	while(TRUE) 
	{ 
		buffer[1025] = {0};
		//clear the socket set 
		FD_ZERO(&readfds); 
	
		//add master socket to set 
		FD_SET(master_socket, &readfds); 
		max_sd = master_socket; 
			
		//add child sockets to set 
		for ( i = 0 ; i < max_clients ; i++) 
		{ 
			//socket descriptor 
			sd = client_socket[i]; 
				
			//if valid socket descriptor then add to read list 
			if(sd > 0) 
				FD_SET( sd , &readfds); 
				
			//highest file descriptor number, need it for the select function 
			if(sd > max_sd) 
				max_sd = sd; 
		} 
	
		//wait for an activity on one of the sockets , timeout is NULL , 
		//so wait indefinitely 
		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL); 
	
		if ((activity < 0) && (errno!=EINTR)) 
		{ 
			printf("select error"); 
		} 
			
		//If something happened on the master socket , 
		//then its an incoming connection 
		if (FD_ISSET(master_socket, &readfds)) 
		{ 
			if ((new_socket = accept(master_socket, 
					(struct sockaddr *)&address, (socklen_t*)&addrlen))<0) 
			{ 
				perror("accept"); 
				exit(EXIT_FAILURE); 
			} 
			
			//inform user of socket number - used in send and receive commands 
		//	cout << "New connection , socket fd is " << new_socket <<  ", ip is : " << inet_ntoa(address.sin_addr) << ", port : " <<  ntohs(address.sin_port) << endl;

			//inform user of socket number - used in send and receive commands 
			cout << "New connection , socket fd is " <<  new_socket << ", ip is : " << inet_ntoa(address.sin_addr) << ", port : " << ntohs(address.sin_port) << endl;  
		
			//send new connection greeting message 
			if( send(new_socket, message, strlen(message), 0) != strlen(message) ) 
			{ 
				perror("send"); 
			} 
				
			puts("Welcome message sent successfully"); 
				
			//add new socket to array of sockets 
			for (i = 0; i < max_clients; i++) 
			{ 
				//if position is empty 
				if( client_socket[i] == 0 ) 
				{ 
					client_socket[i] = new_socket; 
					printf("Adding to list of sockets as %d\n" , i); 
						
					break; 
				} 
			} 
		} 
			
		//else its some IO operation on some other socket 
		for (i = 0; i < max_clients; i++) 
		{ 
			//cout << "Checking socket [" << i << "] for activity" << endl;
			char buffer2[1025] = {0};
			sd = client_socket[i]; 
				
			if (FD_ISSET( sd , &readfds)) 
			{ 
				//Check if it was for closing , and also read the 
				//incoming message
				cout << "Active client [" << i << "]: " << sd << endl; 
				if ((valread = read( sd , buffer2, 1025)) == 0) 
				{ 
					//Somebody disconnected , get his details and print 
					getpeername(sd , (struct sockaddr*)&address ,  
						(socklen_t*)&addrlen); 
					cout << "Host disconnected , ip " << inet_ntoa(address.sin_addr) << ", port " << ntohs(address.sin_port) << endl; 
						
					//Close the socket and mark as 0 in list for reuse 
					close( sd ); 
					client_socket[i] = 0; 
				} 
					
				//Handshake with active client 
				else
				{ 
					string incoming_message = string(buffer2);
cout << "CLIENT CNTRL :" << incoming_message << endl;
					
					if(incoming_message[0] == '0'){
						msg_sz = data_num(incoming_message);
						ky = chk_num(incoming_message);
						//Full message has been sent
						if(srt_bt >= msg_sz && srt_bt > 0){
							cout << "FULL: " << endl;
							cout <<  msg << endl;
							return 0;
						}
						int clt_buff = chk_num(incoming_message);
						buffer2[1025] = {0};
cout << "requesting byte : " << srt_bt << endl;
						string serv_cntrl = serv_control_pkt("", srt_bt, 7);
						send_msg(sd, serv_cntrl);//Send server data request
						incoming_message = timed_listen(sd);//Listen for data packet
cout << "CLIENT DATA: " << incoming_message << endl;
						//Error check
						if(!check_pkt(incoming_message, srt_bt, 7) || incoming_message[0] == '0'){
							errlog[i] += 1;
							if(errlog[i] > 2){//If more than two errors occur on a client, remove it
								FD_CLR(sd, &readfds);
							}
						}
						else{//Receive data
							string data = decryption(data_msg(incoming_message, 7), ky);
cout << "client sent: " << data << endl;
							msg.append(data);
							srt_bt += data.length();
						}
					}
				} 
			} 
		}
	} 	
	return 0; 
} 
