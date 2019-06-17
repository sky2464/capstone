
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
using namespace std;

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

#define PORT 8080 
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
	    printf("%s\n",buffer );

/*
	string message = "Message Received"; 
	message.insert(0, "<header>");
	const char *hello= message.c_str();
*/
	    send(new_socket , hello , strlen(hello) , 0 ); 
	    printf("Hello message sent\n");
    }
    return 0; 
}
