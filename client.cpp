// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h>
#include <string>
#include <sstream>
#include <iostream>

#include <time.h>
#include <math.h>
 
#define PORT 8080 

using namespace std;

struct event{
	clock_t timestamp;
	string message;
};

string to_ascii(char c){
	int ascii = int(c);
	string code = bitset<8>(ascii).to_string();
	return code;
}

string ascii_string(string in){
	string out = "";
	for(int i = 0; i < in.length();i++){
		out.append(in[i]);
	}
	return out;
}

string checkSum(string data){
	substr_a = atoi(code.substr(0, (code.length()/2)-1).c_str());
	substr_b = atoi(code.substr((code.length()/2), (code.length()/2)).c_str());
	int c = substr_a&substr_b;
	return bitset<8>(c).to_string();
}
//Function to create a sequence of chunks to send
vector<string> header(string msg/*, int buff_size*/){
	vector<string> msgs;
	//Data portion
	code = ascii_string(msg);
	//Control message
	string control = "0";
	control.append(bitset<6>(code.length()).to_string());
	control.append(bitset<12>(0).to_string());
	msgs.push_back(control);
	for(int i = 0; i < code.length(); i + 8){
		string id = "1";
		string data = code.substr(i, 7);
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
	cout << "Message: ";
	getline(cin, message);
	
	vector<string> packets = header(message);
	for(int i = 0; i < packets.length(); i++){
		const char *ms = packets[i].c_str();
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
