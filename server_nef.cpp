// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <bitset>
#include <time.h>
#include <math.h>
#include "jana.h"

#define PORT 8080 

using namespace std;

int serv_dos_interface(){
    int select;
    cout << "[HACKERMAN INTERFACE]" << endl;
    cout << "| 1 << Control DOS  |" << endl;//Dont send control packet to client
    cout << "| 2 << Data DOS     |" << endl;//Request incorrect data packet (out of range)
	cout << "| 3 << Request flood|" << endl;//Repeatedly request the same packet
    cout << ">> ";
    cin >> select; 
return select;
}

int main(int argc, char const *argv[]) 
{ 
	int dos = 5;//serv_dos_interface();
	int server_fd, new_socket, valread; 
	struct sockaddr_in address; 
	int opt = 1; 
	int addrlen = sizeof(address); 
	
	// Creating socket file descriptor 
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	// Forcefully attaching socket to the port 8080 
	//if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
	{ 
		//perror("setsockopt"); 
		//exit(EXIT_FAILURE); 
	} 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORT ); 
	
	// Forcefully attaching socket to the port 8080 
	if (bind(server_fd, (struct sockaddr *)&address, 
								sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	if (listen(server_fd, 3) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 
while(true){
	if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
					(socklen_t*)&addrlen))<0) 
	{ 
		perror("accept"); 
		exit(EXIT_FAILURE); 
	} 


	//Server parameters
	int header_len = 7;
	int buffer_size = 15;

	//Accept the client control packet
	//Decode client control packet
	string serv_cntrl = read_msg(new_socket);
cout << "Control packet received: " << serv_cntrl << endl;

	int expects = (data_num(serv_cntrl)/(buffer_size-header_len))+1;//Number of expected packets
	int client_buffer = chk_num(serv_cntrl);//Size of client buffer
cout << "Server expects " << expects << " packets" << endl;

	if(dos == 1){break;}//Nefarious option 1, deny control packet handshake

	//Send server control packet, requests first packet
	string message = "";
	string cntrl = serv_control_pkt(message, 0, buffer_size);
	send_msg(new_socket, cntrl);
cout << "Control packet sent : " << cntrl << endl;

	//Accept incoming data packets and confirm receiving
	vector<string> pkt_data;
	string mssg;
	for(int i = 0; i < expects; i++){
		mssg = timed_listen(new_socket);
		for(int j = 0; j < 2; j++){
			if(mssg[0] == '0'){
				server_request_client(new_socket, i, buffer_size);
				mssg = timed_listen(new_socket);
			}//Error message sent back
			else if(mssg == "null"|| !check_pkt(mssg, expects, i)){
				cout << "Packet not recieved or incorrect" << endl;
				server_request_client(new_socket, i, buffer_size);
				mssg = timed_listen(new_socket);
				if(i == 1){return 0;}
			}//Message is never sent by client, or packet is incorrect
			else{
				cout << "Packet Verified" << endl;
				break;
			}
		}//If next control packet is not sent from server, try again twice
cout << "Data packet received: " << mssg << endl;
		string chunk = data_msg(mssg, header_len);
		pkt_data.push_back(chunk);
		
		if(dos == 3){//Nefarious option 3, send packet request send again repeatedly
			while(true){
				string s_a = serv_control_pkt("", i, buffer_size);
				send_msg(new_socket, s_a);
cout << "Data packet resent: " << mssg << endl;
			}
		}
		
		server_request_client(new_socket, i+1, buffer_size);//Request next packet
cout << "Server requesting packet " << i + 1 << "..." << endl;
		//if(dos == 2){cnfm[1] = '9';}//Nefarious option 2, request invalid packet
	}
	//Print the data
	cout << "Message: ";
	for(int i = 0; i < pkt_data.size(); i++){
		cout << pkt_data[i];
	}
	cout << endl;
}
	return 0;
} 
