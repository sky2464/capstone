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

using namespace std;

void send_msg(int skt, string msg){
	const char * m = msg.c_str();
	send(skt, m, strlen(m), 0);
}

string read_msg(int skt){
	char buffer[1024] = {0};
	int valread = read(skt, buffer, 1024);
	return string(buffer);
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

int data_num(string msg){
    return atoi(msg.substr(1,2).c_str());
}

int chk_num(string msg){
    return atoi(msg.substr(3,4).c_str());
}

string data_msg(string msg, int hdr_ln){
	return msg.substr(hdr_ln, msg.length() - hdr_ln);
}

string timed_listen(int sck){
        time_t ts;
        time(&ts);
        char bfr[1024] = {0};
        string msg = "null";
        double sec;
        while(string(bfr).length() == 0 && sec < 11){
                time_t ct = time(NULL);
                sec = difftime(ct, ts);
                bfr[1024] = {0};
                int valread = read(sck, bfr, 1024);
        }
        if(sec < 11){msg = string(bfr);}
        return msg;
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
		cout << pkt << endl;
	}
	return pkt_list;
}

string serv_control_pkt(string data, int req_num, int buffer_sz){
	string p = "0";
	string len = zero_pad(req_num, 2);//packet number for server to request
	string sz = zero_pad(buffer_sz, 4);//Send buffer size to destination
	p = p + len + sz;
	return p;
}

string clt_control_pkt(string data, int buffer_sz){
	string p = "0";
	string len = zero_pad(data.length(), 2);//max 99 characters message size for two character packet area
	string sz = zero_pad(buffer_sz, 4);//Send buffer size to destination
	p = p + len + sz;
	return p;
}

void server_request_client(int skt, int pkt_num, int buffer_sz){
	string msg = serv_control_pkt("", pkt_num, buffer_sz);
	send_msg(skt, msg);
}

bool client_respond_to_request(int skt, string serv_req, vector<string> pkt_lst){
	int i = data_num(serv_req);
	if(i < pkt_lst.size()){
		send_msg(skt, pkt_lst[i]);
		return true;
	}
	else{
		return false;
	}
}

bool check_pkt(string msg, int expts, int crnt){
	const char * chk_sum = check_sum(data_msg(msg, 7)).c_str();
	if(atoi(chk_sum) == chk_num(msg) && data_num(msg) < expts && data_num(msg) == crnt){
		return true;
	}
	else{
		return false;
	}
}