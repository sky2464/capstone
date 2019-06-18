
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
    if (bind(server_fd, (struct sockaddr *)&address,  
                                 sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    }
	
    string mssg = "";
	
    while(true){
	    if (listen(server_fd, 3) < 0) 
	    { 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	    } 
	    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
		               (socklen_t*)&addrlen))<0) 
	    { 
		perror("accept"); 
		exit(EXIT_FAILURE); 
	    } 
	    char buffer[1024] = {0};
	    valread = read( new_socket , buffer, 1024);
	    string current = string(buffer);
	    event c = decode_msg(current);
	    events.push_back(c);
	    mssg.append(c.message);
	    
	    

/*
	string message = "Message Received"; 
	message.insert(0, "<header>");
	const char *hello= message.c_str();

	    send(new_socket , hello , strlen(hello) , 0 ); 
	    printf("Hello message sent\n");*/
    }
    cout << mssg << "received" << endl;
    return 0; 
}
