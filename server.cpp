
// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <string>
#include <bitset>
#include <vector>
#include <time.h>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <sstream>
 
#define PORT 8080 

using namespace std;

struct event{
	bool packet_type;//True for data packet/ False for control packet
	int userid = 1; //Local count of users
	int packet_num; //Number of packets to send or Number of packet sent
	time_t timestamp;//Timestamp for message
	int check_sum;//Checksum correct
	string message;//Data
};

int checkSum(string data, int check_num, int position, int len){//Checksum function
	string data_sub = data.substr(position,len);//Segment preset chunk of data for checksum
	int data_num = 0;
	for(int i = 0; i < data_sub.length(); i++){
		data_num += int(data_sub[i]);
	}
	int c = check_num&data_num;//XOR
	return c;//Checksum
}

string zero_pad(int num, int len)
{
    ostringstream ss;
    ss << setw( len ) << setfill( '0' ) << num;
    return ss.str();
}

vector<string> packets(string message, int buffer_sz, int head_sz, int checks_val, int checks_pos, int chk_ln){//Entire message, buffer size, header length, checksum value, checksum alg position, checksum alg length
	vector<string> pckts;
	int chunk_sz = buffer_sz - head_sz;
	for(int i = 0; i < message.length()/chunk_sz; i++){//Header creation + data chunk append
		string data_packet = "1";
		string packet_num = zero_pad(i, 2);//Packet number
		string check_sum = zero_pad(checkSum(message.substr(chunk_sz*i, chunk_sz), checks_val, checks_pos, chk_ln), 4);//Checksum
		string chunk = message.substr(chunk_sz*i, chunk_sz);//Data
		string packet = data_packet + packet_num + check_sum + chunk;//Header + data
		pckts.push_back(packet);
	}
	return pckts;
}

event decode_packet(string pckt, int hdr_len){
	event n;
	string header = pckt.substr(0, hdr_len);
	string data = pckt.substr(hdr_len, pckt.length()-hdr_len);
	if(header[0] = '1'){//Data packet
		n.packet_type = true;
		n.packet_num = atoi(header.substr(1,2).c_str());
		n.check_sum = atoi(header.substr(3,4).c_str());
	}
	if(header[0] = '0'){//Control packet
		n.packet_type = false;
		n.packet_num = 0;
		n.check_sum = atoi(header.substr(3,4).c_str());
	}
	//time(n.timestamp);
	n.message = data;
	return n;
}

int main(int argc, char const *argv[]) 
{ 
	int user_count = 0;
	int my_buff_sz = 20;
	vector<event> events;

	int header_len = 10;//Header size + ending char '/'

	int checksum_val = 25;//Arbitrary preset value
	int check_pos = 0;
	int length_ = to_string(checksum_val).length();

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
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                  &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
       
    // Forcefully attaching socket to the port 8080 
    if (bind(server_fd, (struct sockaddr *)&address,  sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    }
	string full_msg;
    
	    if (listen(server_fd, 3) < 0) 
	    { 
			perror("listen"); 
			exit(EXIT_FAILURE); 
	    } 
	while(true){
	    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) 
	    { 
			perror("accept"); 
			exit(EXIT_FAILURE); 
	    } 
	    char buffer[1024] = {0};
	    valread = read( new_socket , buffer, 1024);
	    string current = string(buffer);
		cout << current << endl;
		int client_buffer;

	    if(current[0] == '0'){
			client_buffer = atoi(current.substr(1, current.length()-1).c_str());
			string control_packet = "000";
			control_packet.append(zero_pad(my_buff_sz, 4));
			const char *cntrl = control_packet.c_str();
			send(new_socket, cntrl, strlen(cntrl), 0);
			cout << "Control packet sent with buffer size " << my_buff_sz << endl;
		}
		if(current[0] == '1'){
			event c = decode_packet(current, header_len);
			cout << "Packet Message: " << c.message;
			full_msg.append(c.message);
			string cnfm = "pkt_rcv";
			const char *confirm = cnfm.c_str();
			cout << "pkt_rcv" << endl;
			send(new_socket , confirm , strlen(confirm) , 0 );
		}
    }
	const char *complete = full_msg.c_str();
	cout << "Client said: " << full_msg << endl;
	send(new_socket , complete , strlen(complete) , 0 );
    return 0; 
}
