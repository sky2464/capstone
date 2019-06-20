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
		n.check_sum = atoi(header.substr(1,4).c_str());
	}
	if(header[0] = '0'){//Control packet
		n.packet_type = false;
		n.packet_num = 0;
		n.check_sum = atoi(header.substr(1,4).c_str());
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

	int header_len = 7;//Header size

	int checksum_val = 25;//Arbitrary preset value
	int check_pos = 0;
	int length_ = to_string(checksum_val).length();

	int sock = 0, valread; 
	struct sockaddr_in serv_addr;
	char buffer[1024] = {0}; 
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
	
	string message;
	cout << "Input message: ";
	getline(cin, message);
	cout << "MESSAGE TO BE SENT : [" << message << "]" << endl;
	
	string control_packet = "000";
	control_packet.append(zero_pad(my_buff_sz, 4));
	const char *cntrl = control_packet.c_str();
	send(sock, cntrl, strlen(cntrl), 0);
	cout << "Control packet sent with buffer size " << my_buff_sz << endl;
	//Client sends control packet

	valread = read(sock, buffer, 1024);
	printf("%s\n", buffer);
	string server_cntrl = string(buffer);
	//Server sends control packet to server
	event serv_cntrl = decode_packet(server_cntrl, header_len);
	//Client decodes control message
	int server_buffer_len = serv_cntrl.check_sum;
	cout << "Server buffer length: " << server_buffer_len << endl;

	cout << "Sending data packets" << endl;
	vector<string> p = packets(message, my_buff_sz, header_len, checksum_val, check_pos, length_);

	for(int i = 0; i < p.size(); i++){
		cout << p[i] << endl;
		const char *m = p[i].c_str();
		send(sock, m, strlen(m), 0);
	}//Send Data packets

	
	return 0; 
} 
