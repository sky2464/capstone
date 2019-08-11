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

#define PORT 8080 

using namespace std;

string zero_pad(int num, int len);

string check_sum(string data);
vector<string> data_packets(string data, int buffer_sz, int header_len);


string control_pkt(string data, int buffer_sz);



int main(int argc, char const *argv[]) 
{ 
	int server_fd, new_socket, valread; 
	struct sockaddr_in address; 
	int opt = 1; 
	int addrlen = sizeof(address); 
	
	// Creating socket file descriptor 
	// socket descriptor, an integer (like a file-handle)
	//AF_INET (IPv4 protocol)
	//SOCK_STREAM is communication type
	//Protocol value for Internet Protocol(IP), which is 0. This is the same number which appears on protocol field in the IP header of a packet

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	// Forcefully attaching socket to the port 8080 
	//if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
	//{ 
		//perror("setsockopt"); 
		//exit(EXIT_FAILURE); 
	//} 
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
	char buffer[1024] = {0}; 
	valread = read(new_socket, buffer, 1024);
	//Decode client control packet
	string serv_cntrl = string(buffer);
cout << "Control packet received: " << serv_cntrl << endl;
	int expects = (atoi(serv_cntrl.substr(1, 2).c_str())/(buffer_size-header_len))+1;//Number of expected packets
	int client_buffer = atoi(serv_cntrl.substr(3, 4).c_str());//Size of client buffer
	//Send server control packet
	string message = "";
	string cntrl = control_pkt(message, buffer_size);
	const char *control = cntrl.c_str();
	send(new_socket, control, strlen(control), 0);
cout << "Control packet sent : " << cntrl << endl;
	//Accept incoming data packets and confirm receiving
	vector<string> pkt_data;
	for(int i = 0; i < expects; i++){
		char buffer1[1024] = {0}; 
		valread = read(new_socket, buffer1, 1024);
cout << "Data packet received: " << string(buffer1) << endl;
		string chunk = string(buffer1).substr(7, buffer_size - header_len);
		pkt_data.push_back(chunk);
		//Error checks here (packet number and checksum)
		if(string(buffer1).substr(3,4) != check_sum(chunk) || atoi(string(buffer1).substr(1,2).c_str()) != i){
			string s_a = "s/a";
			send(new_socket, s_a.c_str(), strlen(s_a.c_str()), 0);
			pkt_data.pop_back();
			buffer1[1024] = {0}; 
			valread = read(new_socket, buffer1, 1024);
cout << "Data packet resent: " << string(buffer1) << endl;
			chunk = string(buffer1).substr(7, buffer_size - header_len);
			pkt_data.push_back(chunk);
		}
		string cnfm = "cnfm";
		send(new_socket, cnfm.c_str(), strlen(cnfm.c_str()), 0);//Send packet confirmation
	}
	//Print the data
	cout << "Message: ";
	for(int i = 0; i < pkt_data.size(); i++){
		cout << pkt_data[i];
	}
	cout << endl;
}

	// valread = read( new_socket , buffer, 1024); 
	// printf("%s\n",buffer ); 
	// send(new_socket , hello , strlen(hello) , 0 ); 
	// printf("Hello message sent\n"); 
	return 0;
} 


string zero_pad(int num, int len){//Pad a number with 0s
    ostringstream ss;
    ss << setw( len ) << setfill( '0' ) << num;
    return ss.str();
}

string check_sum(string data){
	int check_val = 256;//Arbitrary assigned value
	int data_val = 0;
	for(int i = 0; i < data.length(); i++){
		data_val += int(data[i]);
	}
	//int xor_ = check_val&data_val;
	return zero_pad(data_val, 4);//zero_pad(xor_, 4);
}

vector<string> data_packets(string data, int buffer_sz, int header_len){
	vector<string> pkt_list;
	int data_chunk = buffer_sz - header_len;
	for(int i = 0; i < (data.length()/data_chunk) + 1; i++){
		string chunk = data.substr(data_chunk*i, data_chunk);
		string pkt_num = zero_pad(i, 2);
		string chk_sum = check_sum(chunk);
		string pkt = "1" + pkt_num + chk_sum;
		pkt_list.push_back(pkt);
	}
	return pkt_list;
}

string control_pkt(string data, int buffer_sz){
	string p = "0";
	string len = zero_pad(data.length(), 2);//max 99 characters message size for two character packet area
	string sz = zero_pad(buffer_sz, 4);//Send buffer size to destination
	p = p + len + sz;
	return p;
}

