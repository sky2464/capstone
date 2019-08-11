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

#define PORT 8080 

using namespace std;

string zero_pad(int num, int len);

string check_sum(string data);

vector<string> data_packets(string data, int buffer_sz, int header_len);

string control_pkt(string data, int buffer_sz);

int main(int argc, char const *argv[]) 
{ 
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
	
	//Client parameters
	int header_len = 7;
	int buffer_size = 10;

	//Take user input message
	string message;
	cout << ">" ;
	getline(cin, message);
	//Send client control packet
	string cntrl = control_pkt(message, buffer_size);
	const char *control = cntrl.c_str();
	send(sock, control, strlen(control), 0);
cout << "Control packet sent : " << cntrl << endl;
	//Receive server control packet
	char buffer[1024] = {0}; 
	valread = read(sock, buffer, 1024);
	//Decode server control packet
	string serv_cntrl = string(buffer);
cout << "Control packet received: " << serv_cntrl << endl;
	//int expects = (atoi(serv_cntrl.substr(1, 2))/(buffer_size-header_len))+1;
	int serv_buffer = atoi(serv_cntrl.substr(3, 4).c_str());
	//Prepare data packets and send them
	vector<string> data_pkts = data_packets(message, serv_buffer, header_len);
	for(int i = 0; i < data_pkts.size(); i++){
		string t = data_pkts[i];
//t[5] = '2'; //Modify checksum to cause error
//t[1] = '2'; //Modify packet number to cause error
		const char *temp = t.c_str();
		send(sock, temp, strlen(temp), 0);
cout << "Data packet sent : " << t << endl;
		//Busy waiting for server confirmation
		while(true){
			char buff[1024] = {0};
			valread = read(sock, buff, 1024);
			if(string(buff) == "cnfm"){
				break;
			}
			if(string(buff) == "s/a"){
t = data_pkts[i];//Correct packet
temp = t.c_str();//
				send(sock, temp, strlen(temp),0);
cout << "Resent packet : " << temp << endl;
			}
		}
	}

	// send(sock , hello , strlen(hello) , 0 ); 
	// printf("Hello message sent\n"); 
	// valread = read( sock , buffer, 1024); 
	// printf("%s\n",buffer ); 
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
		string pkt = "1" + pkt_num + chk_sum + chunk;
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
