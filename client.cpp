// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <bitset>

#include <time.h>
#include <math.h>
 
#define PORT 8080 

using namespace std;

int user_count = 0;

struct event{
	bool packet_type;//True for data packet/ False for control packet
	int userid; //Local count of users
	int packet_num; //Number of packets to send or Number of packet sent
	clock_t timestamp;//Timestamp for message
	bool checksum_valid;//Checksum correct
	string message;//Data
};

vector<event> events;

string checkSum(string data){//Checksum function
	int substr_a = atoi(data.substr(0, (data.length()/2)-1).c_str());
	int substr_b = atoi(data.substr((data.length()/2), (data.length()/2)).c_str());
	int c = substr_a&substr_b;//XOR halves
	return bitset<8>(c).to_string();
}

event decode_msg(string m){
	event n;
	time(&n.timestamp);
	if(m[0] == '1'){//Decode data packet
		n.packet_type = true;
		n.userid = user_count;
		n.packet_num = stoi(m.substr(1,6).c_str());
		if(checkSum(m.substr(10,8)) == m.substr(6, 4)){n.checksum_valid = true;}
		else{n.checksum_valid = false;}
		n.message = char(stoi(m.substr(10,8).c_str()));
	}
	if(m[0] == '0'){//Decode control packet
		n.packet_type = false;
		n.userid = user_count;
		n.packet_num = stoi(m.substr(1,6).c_str());
		if(checkSum(m.substr(10,8)) == m.substr(6, 4)){n.checksum_valid = true;}
		else{n.checksum_valid = false;}
		n.message = "";
	}
	return n;
}

string ascii_char(string a){//8 bit ascii string to one character string
	string b = " ";
	b[0] = char(stoi(a, nullptr, 2)); 
	return b;
}

string ascii_to(string ascii){//Converts ascii binary string to a regular string
	string msg = "";
	for(int i = 0; i < ascii.length(); i + 8){
		msg.append(ascii_char(ascii.substr(i, 8)));
	}
	return msg;
}

string to_ascii(char c){//Converts char to ascii binary string
	int ascii = int(c);
	string code = bitset<8>(ascii).to_string();
	return code;
}

string ascii_string(string in){//Convert a string to an ascii binary string
	string out = "";
	for(int i = 0; i < in.length();i++){
		out.append(to_ascii(in[i]));
	}
	return out;
}

//Function to create a sequence of chunks to send
vector<string> header(string msg/*, int buff_size*/){
	vector<string> msgs;
	//Data portion
	string code = ascii_string(msg);
	//Control message
	string control = "0";
	control.append(bitset<6>(msg.length()).to_string());
	control.append(bitset<12>(0).to_string());
	msgs.push_back(control);
	for(int i = 0; i < code.length(); i + 8){
		string id = "1";
		string data = code.substr(i, 8);
		string chk = checkSum(data);
		id.append(chk);
		id.append(data);
		msgs.push_back(id);	
	}
	return msgs;
}

int main(int argc, char const *argv[]) 
{ 
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
	
	vector<string> packets = header(message);
	for(int i = 0; i < packets.size(); i++){
		const char *ms = packets[i].c_str();
		printf(ms);
		send(sock, ms, strlen(ms), 0);
	}/*
	cout << message << endl;
	message.insert(0, "<header>");
	const char *hello= message.c_str();

	send(sock , hello , strlen(hello) , 0 ); 
	printf("Message sent\n"); */
	valread = read( sock , buffer, 1024); 
	printf("%s\n",buffer ); 
	return 0; 
} 
