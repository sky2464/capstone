
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
	vector<string> p = packets(message, server_buff_len, header_len, checksum_val, check_pos, length_);
	cout << p.size() << endl;
	for(int i = 0; i < p.size(); i++){
		cout << p[i] << endl;
		const char *m = p[i].c_str();
		send(sock, m, strlen(m), 0);
		while(true){//Busy waiting for data receival confirmation
			char buff[1024] = {0};
			valread = read(sock, buff, 1024);
			string cnfm = string(buff);
			if(cnfm == "pkt_rcv"){
				break;
			}
		}
	}//Send Data packets

	
	return 0; 
} 
