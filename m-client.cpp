// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <bitset>
#include <time.h>
#include <math.h>
#include <fstream>
#include "jana-m.h"

#define PORT 8888

using namespace std;

// string dec_to_hex(int i, int len){

// }

string message_to_be_sent(string filename){
	ifstream input;
	input.open(filename);
	string m;
	while(!input.eof()){
		string temp;
		input >> temp;
		m.append(temp);
	}
	return m;
}

void handshake(int sock, string in, int limit, int err){
	if(err < 2){	
		//Send control packet to indicate client is ready
		string clt_cntrl = clt_control_pkt(in, 7);
		send_msg(sock, clt_cntrl);
		//Receive the server's data request
		string srv = timed_listen(sock);
	cout << "RECEIVED: " << srv << endl;
		int srt = data_num(srv);
		if(srt > in.length() || srv == "null"){
			handshake(sock, in, limit, err+1);
		}
		//Send the requested data
		string data = clt_data_pkt(in, limit, srt);
		send_msg(sock, data);
	cout << "SENDING: " << data << endl;
	}
}

int main(int argc, char const *argv[]) 
{ 
	int dos = 5;//clt_dos_interface();
	int buffer_s = 4;

	int sock = 0, valread; 
	struct sockaddr_in serv_addr; 
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 
	
	// Convert IPv4 and IPv6 addresses from text to binary form 
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return -1; 
	}
	//Connecting message
cout << timed_listen(sock) << endl;

	string in = message_to_be_sent("text.txt");
	int len = in.length();
	int j = 0;
cout << "Message to be sent: " << in << endl;//Full message
int sz;
cout << "Enter client data transfer size: " ; 
cin >> sz;

	//Start communication
	string step;
	while(cin.get() == '\n'){
		handshake(sock, in, sz, 0);
	}

	return 0; 
} 