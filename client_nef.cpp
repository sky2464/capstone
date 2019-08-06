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
#include "jana.h"

#define PORT 8080 

using namespace std;

int clt_dos_interface(){
    int select;
    cout << "[HACKERMAN INTERFACE]" << endl;
    cout << "| 1 << Control DOS  |" << endl;//Send incorrect value for size of message in control packet
    cout << "| 2 << Data DOS     |" << endl;//Dont send correct data packet repeatedly
    cout << ">> ";
    cin >> select;
	return select; 
}

int main(int argc, char const *argv[]) 
{ 
	int dos = 5;//clt_dos_interface();

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
	string tempy;
	cout << ">" ;
	cin >> tempy;
	getline(cin, message);
	message.insert(0, tempy);

	//Send client control packet
	string cntrl = clt_control_pkt(message, buffer_size);
	if(dos == 1){cntrl[1] = '5';}//Nefarious option 1, give incorrect message size
	send_msg(sock, cntrl);
cout << "Control packet sent : " << cntrl << endl;

	//Receive server control packet
	string serv_cntrl = timed_listen(sock);
	for(int i = 0; i < 2; i++){
		if(serv_cntrl == "null"){
			send_msg(sock, cntrl);
			serv_cntrl = timed_listen(sock);
			if(i = 1){return 0;}
		}
		else{
			break;
		}
	}//If control packet is not sent from server, try again twice
cout << "Control packet received: " << serv_cntrl << endl;

	int serv_buffer = chk_num(serv_cntrl);
cout << "Server buffer length: " << serv_buffer << endl;

	//Prepare data packets and send them
	vector<string> data_pkts = data_packets(message, serv_buffer, header_len);
cout << "<Packets created>" << endl;

	vector<int> pkt_sent_cnt;
	pkt_sent_cnt.reserve(data_pkts.size()+1);
	pkt_sent_cnt = {0};
cout << "Client to send " << data_pkts.size() << " packets" << endl;

	string mssg = serv_cntrl;
	while(true){
		if(!client_respond_to_request(sock, mssg, data_pkts)){//Checks if a valid packet request
			for(int i = 0; i < 2; i++){
				send_msg(sock, zero_pad(0, header_len+serv_buffer));//Send error message back
				mssg = timed_listen(sock);
				if(mssg == "null" || !client_respond_to_request(sock, mssg, data_pkts)){//Try again
	cout << "Tried getting control packet again" << endl;
					if(i = 1){
						cout << "Tried to receive twice" << endl;
						return 0;
					}
				}
				else{
					break;
				}
			}//If next control packet is not sent from server, try again twice
		}
cout << "Server request packet " << data_num(mssg) << endl;
cout << "Data packet sent : " << data_pkts[data_num(mssg)] << endl;
cout << "Sent before [" << pkt_sent_cnt[data_num(mssg)] << "] times" << endl;
		if(data_num(mssg) == data_pkts.size()-1){
			cout << "All packets sent" << endl;
			break;
		}//If full message has already been sent, end connection
		
		pkt_sent_cnt[data_num(mssg)] += 1;
		if(pkt_sent_cnt[data_num(mssg)] > 2){
			cout << "Server requested too many times" << endl;
			break;
		}//If server requests a packet more than twice, end connection

		mssg = timed_listen(sock);//Accept next control packet data request
		for(int i = 0; i < 2; i++){
			if(mssg == "null"){
				send_msg(sock, zero_pad(0, header_len+serv_buffer));//Send error message back
				mssg = timed_listen(sock);
				if(i = 1){
					cout << "Tried to receive twice" << endl;
					return 0;
				}
			}
			else{
				break;
			}
		}//If next control packet is not sent from server, try again twice
	}
	return 0; 
} 
