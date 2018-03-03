#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <string.h>

#include <ctype.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <netdb.h>
#include "FileManager.h"

#include <iostream>
#include <sstream>

#include <bits/stdc++.h>

#define PORT 8001

using namespace std;

#define READ_FD     0
#define WRITE_FD    1

char PADBYTE = '~';
int OCTOBLOCK_SIZE = 8888;
int OCTOLEG_SIZE = 1111;

string intToString(int i) {
	stringstream ss;
	ss << i;
	return ss.str();
}

int stringToInt(string s) {
	stringstream ss(s);
	int i;
	ss >> i;
	return i;
}

string getSequence(int num) {
	string s = "00000000";
	s[num] = '1';
	return s;
}

// Function to convert decimal number to octal
int decimalToOctal(int decimalNumber) {
	int rem, i = 1, octalNumber = 0;
	while (decimalNumber != 0) {
		rem = decimalNumber % 8;
		decimalNumber /= 8;
		octalNumber += rem * i;
		i *= 10;
	}
	return octalNumber;
}

//string generateResponse(string request) { // TODO: also return a ACK#
//
//	string type;
//	string filename = "";
//	string response = "INVALID REQUEST";
//
//	unsigned int i = 0;
//	for (; i < request.size(); i++) {
//		type += request[i];
//		if (request[i] == ':') {
//			break;
//		}
//	}
//
//	i++; //skip passed ":"
//	FileManager fileManager;
//	int blockSize = 8888;
//	int blockNumber;
//	int legSize = 1111;
//	int legNumber;
//	int miniSize = 8;
//	int miniNumber;
//	string bN, lN, mN = "";
//	if (type == "OctoCheck:") {
//		for (; i < request.size(); i++) {
//			filename += request[i];
//		}
//		int size = fileManager.getFileSize(filename);
//		response = intToString(size);
//	} else if (type == "OctoGetBlock:") {
//		for (; request[i] != ' '; i++) {
//			filename += request[i];
//		}
//		i++;
//		for (; i < request.size(); i++) {
//			bN += request[i];
//			blockNumber = stringToInt(bN);
//		}
//
//		response = fileManager.getFileRange(filename, (blockNumber), blockSize);
//
//	} else if (type == "OctoGetLeg:") {
//		for (; request[i] != ' '; i++) {
//			filename += request[i];
//		}
//		i++;
//		for (; request[i] != ' '; i++) {
//			bN += request[i];
//			blockNumber = stringToInt(bN);
//		}
//		for (; i < request.size(); i++) {
//			lN += request[i];
//			legNumber = stringToInt(lN);
//		}
//
//		response = fileManager.getFileRange(filename,
//				((blockNumber * blockSize) + (legNumber * legSize)), legSize)
//				+ intToString(decimalToOctal(legNumber));
//
//	} else if (type == "OctoGetMinis:") {
//
//		for (; request[i] != ' '; i++) {
//			filename += request[i];
//		}
//		i++; //skip past ' '
//		for (; request[i] != ' '; i++) {
//			bN += request[i];
//			blockNumber = stringToInt(bN);
//		}
//		i++;
//		for (; request[i] != ' '; i++) {
//			lN += request[i];
//			legNumber = stringToInt(lN);
//
//		}
//		for (; i < request.size(); i++) {
//			mN += request[i];
//			miniNumber = stringToInt(mN);
//		}
//
//		response = fileManager.getFileRange(filename,
//				((blockNumber * blockSize) + (legNumber * legSize)),
//				(miniNumber * miniSize));
//		response = fileManager.addPadding(response, response.size());
//		cout << "mini response:" << endl;
//		cout << response << endl;
//
//	} else if (type == "OctoGetLegPartial") {
//
//		int startNum, endNum, sequenceNum;
//		string sN, eN, seN = "";
//		for (; request[i] != ' '; i++) {
//			filename += request[i];
//		}
//		i++; //skip past ' '
//		for (; request[i] != ' '; i++) {
//			bN += request[i];
//			blockNumber = stringToInt(bN);
//		}
//		i++;
//		for (; request[i] != ' '; i++) {
//			sN += request[i];
//			startNum = stringToInt(sN);
//
//		}
//		i++;
//		for (; request[i] != ' '; i++) {
//			sN += request[i];
//			startNum = stringToInt(sN);
//
//		}
//		i++;
//		for (; i < request.size(); i++) {
//			seN += request[i];
//			sequenceNum = stringToInt(seN);
//		}
//
//		response = fileManager.getFileRange(filename, startNum,
//				(endNum - startNum));
//
//		response = fileManager.addPadding(response, response.size());
//
//		response += intToString(decimalToOctal(sequenceNum));
//
//	} else if (type == "OctoGetTiny:") {
//
//		int sequenceNum, tinyNum;
//		string tN, sN = "";
//		for (; request[i] != ' '; i++) {
//			filename += request[i];
//		}
//		i++; //skip past ' '
//		for (; request[i] != ' '; i++) {
//			tN += request[i];
//			tinyNum = stringToInt(bN);
//		}
//		i++; //skip past ' '
//		for (; i < request.size(); i++) {
//			sN += request[i];
//			sequenceNum = stringToInt(sN);
//		}
//
//		if (tinyNum == -1) {
//			response = PADBYTE;
//		} else {
//			response = fileManager.getFileRange(filename, tinyNum, 1);
//		}
//
//		response = fileManager.addPadding(response, response.size());
//
//		response += intToString(decimalToOctal(sequenceNum));
//	}
//
//	return response;
//
//}

void reply(string response, int sock, struct sockaddr_in client_address) {
	unsigned int client_address_len = sizeof(client_address);
	int n = response.length();
	char char_array[n];
	strcpy(char_array, response.c_str());
	int sent_len = sendto(sock, char_array, n + 1, 0,
			(struct sockaddr *) &client_address, client_address_len);
	printf("server sent back message of size:%d\n", sent_len);
	//printf("server sent back message containing:%s\n", response);

}

void listenForMsg(int sock, struct sockaddr_in client_address, char* buffer) {
	// read content into buffer from an incoming client
	unsigned int client_address_len = sizeof(client_address);
	int len = recvfrom(sock, buffer, sizeof(buffer), 0,
			(struct sockaddr *) &client_address, &client_address_len);

	// inet_ntoa prints user friendly representation of the
	// ip address
	buffer[len] = '\0';
	printf("received: '%s' from client %s on port %d\n", buffer,
			inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

}

void setTimeout(int milliseconds) {
	clock_t end;
// a current time of milliseconds
	clock_t start = clock() * 1000 / CLOCKS_PER_SEC;
	int gap;
	do {
		end = clock() * 1000 / CLOCKS_PER_SEC;
		gap = (int) end - start;
		//cout <<"gap: "<<gap<<endl;
	} while (gap < milliseconds);
	//cout <<"timeout done"<<endl;
}

bool isValidID(string msg) {
	if (msg.size() == 4) {
		return true;
	}
}
//
//bool waitForACK(string packetIDstr, int sock,
//		struct sockaddr_in client_address) {
//
//	string msg = "";
//	int packetID = stringToInt(packetIDstr);
//	string ACKstr = intToString(packetID + 1);
//
//	int check = 100;
//	while (check < 100) {
//		check++;
//		char buffer[500];
//		listenForMsg(sock, client_address, buffer);
//		string str(buffer);
//		msg = str;
//		cout << "expecting " << ACKstr << " received " << msg << endl;
//
//		if (isValidID(msg)) {
//			int recvdID = stringToInt(msg);
//			int ACK = stringToInt(ACKstr);
//			if (recvdID > ACK && (recvdID - ACK) < 8) {
//				break;
//			}
//		}
//
//	}
//
//	return (check < 100);
//}

void listenForACK(string packetID, int sock, struct sockaddr_in client_address,
		bool &heard) {

	int fd[2];
	pipe(fd);
	int pid = fork();
	if (pid == 0) {
		close(fd[READ_FD]);

		/* Child 1 Process */
		cout << "child 1 process started" << endl;

		string msg = "";
		string ACK = intToString((stringToInt(packetID) + 1));
		bool recvd = false;
		//write(fd[1], &recvd, sizeof(bool));
		int listenerTimeouts;
		int maxListenerTimeouts = 2;
		while (1) {

			//sleep(2);
			char buffer[500];
			cout << "child 1 listening for ACK..." << endl;
			write(fd[1], &recvd, sizeof(bool));
			listenForMsg(sock, client_address, buffer);

			string str(buffer);


			int i = 0;
			for (; i<str.length();i++){
				if (str[i]==' '){
					break;
				}
				msg+=str[i];
			}


			cout << "child 1 expecting " << ACK << " received " << msg << endl;
			if (msg == ACK) {
				recvd = true;
				write(fd[1], &recvd, sizeof(bool));
				break;
			}
			if (msg == "") {
				cout << "listener timed out" << endl;
				listenerTimeouts++;
			}
			if (listenerTimeouts > maxListenerTimeouts) {
				cout << "max timeout reached" << endl;
				recvd = false;
				write(fd[1], &recvd, sizeof(bool));
				break;
			}
		}
		int c1 = 1;
		write(fd[1], &recvd, sizeof(bool));
		//write(fd[1], &c1, sizeof(int));
		close(fd[1]);
		heard = recvd;
		cout << "finished child 1 process" << endl;
		//exit(-1);
	} else if (pid < 0) {
		printf("Error.\n");
	} else {
		/* Parent Process */
		int fd2[2];
		pipe(fd2);
		int pid2 = fork();
		if (pid2 == 0) {
			/* Child 2 Process */
			cout << "starting child 2 process" << endl;

			int c2 = 3;
			string test = "a";
			//while (test.size() < 5) {
			sleep(5);
			close(fd2[0]);
			write(fd2[1], &test, sizeof(string));
			test += "a";
			cout << "test: " << test << endl;
			//}
			cout << "exiting child 2 process" << endl;
			close(fd2[1]);
			//exit(-1); //or else timer will never stop repeating
		} else if (pid2 > 0) {
			/* Parent Process */
			close(fd[WRITE_FD]);
			close(fd2[WRITE_FD]);
			bool recvd = false;
			string s = "";
			//while (s.size() < 5) {
			s += "a";
			cout << "PARENT waiting for timeout" << endl;
			read(fd2[0], &s, sizeof(string));
			cout << "PARENT read timeout " << s << endl;
			cout << "PARENT waiting for ACK " << endl;
			read(fd[0], &recvd, sizeof(bool));
			cout << "PARENT read ACK " << recvd << endl;

			cout << "PARENT: ACK: " << recvd << " check# " << s << endl;
			if (recvd == true) {
				cout << "PARENT RECVD ACK" << endl;
				//break;
			}
			//}
			cout << "EXITING PARENT" << endl;
			close(fd[READ_FD]);
			close(fd2[READ_FD]);
			heard = recvd;

		}
	}
}

bool addToOrder(vector<bool> orderArray, int j){
	for (int i = 0; i<j; i++){
		if (orderArray[i]==false){
			cout <<i<<" comes before "<<j<<endl;
			return false;
		}
	}
	orderArray[j]=true;
	return true;
}

void multiResponse(string request, int sock,
		struct sockaddr_in client_address) {

	string type;
	string filename = "";
	string response = "INVALID REQUEST";

	unsigned int i = 0;
	for (; i < request.size(); i++) {
		type += request[i];
		if (request[i] == ':') {
			break;
		}
	}

	i++; //skip passed ":"
	FileManager fileManager;
	int blockSize = 8888;
	int blockNumber;
	int legSize = 1111;
	int legNumber;
	int miniSize = 8;
	int miniNumber;

	string bN, lN, mN = "";
	if (type == "OctoCheck:") {
		int maxAttempts = 1;
		int attempts = 0;
		string packetIDstr = "";
		int blockNum;
		for (; request[i] != ' '; i++) {
			filename += request[i];
		}
		i++;
		for (; i<request.size(); i++) {
			packetIDstr += request[i];
		}

		int size = fileManager.getFileSize(filename);
		response = intToString(size) + packetIDstr;
		bool complete = false;
		do {
			attempts++;
			reply(response, sock, client_address);
			listenForACK(packetIDstr, sock, client_address, complete);
			if (complete) {
				cout << "OctoCheck ACK received" << endl;
			} else {
				cout << "no ACK received" << endl;
			}
		} while (!complete && (attempts < maxAttempts));
		if (attempts == maxAttempts) {
			cout << "**Server Timeout**" << endl;
		}
	} else if (type == "OctoGetFullLegs:") {
		int filesize, blockNum;
		string bN;
		string fs, packetIDstr = "";
		for (; request[i] != ' '; i++) {
			filename += request[i];
		}
		cout<<"filename"<<endl;
		cout<<filename<<endl;
		i++;
		for (; request[i] != ' '; i++) {
			fs += request[i];
		}
		cout<<"filesize"<<endl;
		cout<<fs<<endl;
		filesize = stringToInt(fs);

		i++;
		for (; request[i] != ' '; i++) {
			packetIDstr += request[i];
		}
		cout<<"packetIDstr"<<endl;

		i++;
		for (; i < request.size(); i++) {
			bN += request[i];
		}
		cout <<"blockNum: "<<bN<<endl;
		blockNum= stringToInt(bN);

		int packetID = stringToInt(packetIDstr);
		int completedBlocks = (filesize / OCTOBLOCK_SIZE);
		int start = 0;
		int octaLegSize = 1111;
		int maxAttempts = 4;
		int attempts = 0;
		bool recvd = false;
		vector<bool> orderArray(false,8);
		for (int i = 0; i < completedBlocks; i++) { //octablocks
			do {
				attempts++;
				for (int j = 0; j < 8; j++) { //octalegs
					start = (i * 8888) + (j * 1111);
					response = fileManager.getFileRange(filename, start,
							octaLegSize) + getSequence(j)
							+ intToString(packetID) + " " + intToString(blockNum);
					cout << "FullBlock: start " << start << " end:"
							<< start + octaLegSize << " i: " << i << endl;
					cout << "sending leg # " << j << " from block " << i
							<< endl;
					//cout << "leg contents:" << endl;
					//cout << response << endl;
					reply(response, sock, client_address);
				}
				listenForACK(packetIDstr, sock, client_address, recvd);
				if (recvd) {
					cout << "received ACK for octoblock " << i << endl;
					if (addToOrder(orderArray, i)){
						cout<<"order updated"<<endl;
					} else {
						cout<<"out of order"<<endl;
						recvd=false;
					}
				} else {
					cout << "no ACK received" << endl;
				}

			} while (!recvd && (attempts < maxAttempts));
			if (attempts >= maxAttempts) {
				cout << "**Server Timeout**" << endl;
				break;
			} else {
				cout <<"INCREMENTING WHEN, recvd: "<<recvd<<" attempts:"<<attempts<<endl;
				cout << "packetID: " << packetID<< endl;
				packetID=packetID+1;
				cout << "packtID++" << packetID << endl;
				//packetID++; //update after every full block is sent
				cout <<"blockNum: "<<intToString(blockNum)<<endl;
				blockNum=blockNum+1;
				cout <<"blockNum++: "<<intToString(blockNum)<<endl;
				packetIDstr = intToString(packetID);
				attempts = 0;
				i = 0;
			}
		}

	} else if (type == "OctoGetPartLegs:") {
		int filesize;
		string fs, packetID, cB = "";
		for (; request[i] != ' '; i++) {
			filename += request[i];
		}
		i++;
		for (; request[i] != ' '; i++) {
			fs += request[i];
		}
		filesize = stringToInt(fs);

		i++;

		for (; request[i] != ' '; i++) {
			packetID += request[i];
		}

		int completedBlocks = (filesize / OCTOBLOCK_SIZE);
		int completedSize = completedBlocks * OCTOBLOCK_SIZE;
		int remainingSize = filesize - completedSize;
		int partialSize = remainingSize - (remainingSize % 8);
		int octaLegSize = partialSize / 8;
		int start = completedSize;
		for (int i = 0; i < 8; i++) {

			response = fileManager.getFileRange(filename, start, octaLegSize)
					+ getSequence(i) + packetID + "0";
			cout << "server's payload size: " << response.size() << endl;
			cout << "Partblock: start " << start << " end:"
					<< start + octaLegSize << "i=" << i << endl;
			start += octaLegSize;
			reply(response, sock, client_address);
		}

	} else if (type == "OctoGetTinyLegs:") {
		int filesize; // will be between 1 and 7
		string fs, packetID = "";
		for (; request[i] != ' '; i++) {
			filename += request[i];
		}
		i++;
		for (; request[i] != ' '; i++) {
			fs += request[i];
		}
		filesize = stringToInt(fs);
		i++;
		for (; i < request.size(); i++) {
			packetID += request[i];
		}

		int completedSize = (filesize / OCTOBLOCK_SIZE) * OCTOBLOCK_SIZE;
		int remainingSize = filesize - completedSize;
		int partialSize = remainingSize - (remainingSize % 8);
		int tinyLegs = remainingSize % 8;
		int i = 0;
		for (; i < tinyLegs; i++) {
			int start = (completedSize + partialSize) + (i * 1);
			int end = start + 1;
			response = fileManager.getFileRange(filename, start, 1)
					+ getSequence(i) + packetID;
			cout << "Tinyblock: start " << start << " end:" << start + 1;
			reply(response, sock, client_address);
		}

		for (; i < 8; i++) {
			response = PADBYTE + getSequence(i) + packetID;
			reply(response, sock, client_address);
		}
	}

}

int main(int argc, char *argv[]) {

	//FileManager fileManager;
	//string filename = "32KB.txt";
	//cout << "size: " << fileManager.getFileSize(filename) << endl;
//	int diff = 27427 - 26664;
//	string c = fileManager.getFileRange(filename, 0, 8888);
//	string c1 = fileManager.getFileRange(filename, 8888, 8888);
//string c2 = fileManager.getFileRange(filename, 7777, 1111);
//string b = fileManager.getFileRange(filename, 8888, 1111);
//string b2 = fileManager.getFileRange(filename, 9999, 7777);
	//cout << "E" << endl;
	//cout << c + c1 << endl;
//	string c = fileManager.getFileRange(filename, 26664, 32768-26664);
//fileManager.getFileRange(filename, 1111, 1111)+
//			//fileManager.getFileRange(filename, 26664, (27427-26665));
//	cout << "c" << endl;
//	cout << c << endl;
//	string a = fileManager.getFileRange(filename, 26664, 743);
//	cout << "a" << endl;
//	cout << a << endl;
//	string b = fileManager.getFileRange(filename, 26664, 1);
//	cout << "E" << endl;
//	cout << c+a << endl;
	//int test = fileManager.getFileSize(fileName);
	//string fileContents = "";
	//int fileSize = -1;
	//fileManager.findFile(fileName, fileContents, fileSize);

	// port to start the server on
	int SERVER_PORT = PORT;

	// socket address used for the server
	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;

	// htons: host to network short: transforms a value in host byte
	// ordering format to a short value in network byte ordering format
	server_address.sin_port = htons(SERVER_PORT);

	// htons: host to network long: same as htons but to long
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);

	// create a UDP socket, creation returns -1 on failure
	int sock;
	if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("could not create socket\n");
		return 1;
	}
	printf("server socket created\n");
	// bind it to listen to the incoming connections on the created server
	// address, will return -1 on error
	if ((bind(sock, (struct sockaddr *) &server_address, sizeof(server_address)))
			< 0) {
		printf("could not bind socket\n");
		return 1;
	}
	printf("binding was successful\n");
	// socket address used to store client address
	struct sockaddr_in client_address;
	unsigned int client_address_len = sizeof(client_address);
	char client_name[100];

	//--add timer to recvfrom--
//	struct timeval timeout = { 5, 0 }; //set timeout for 2 seconds
////
////	/* set receive UDP message timeout */
//
//	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*) &timeout,
//			sizeof(struct timeval));
	//---------------------------

	// run indefinitely
	while (true) {
		char buffer[1000];

		// read content into buffer from an incoming client
		int len = recvfrom(sock, buffer, sizeof(buffer), 0,
				(struct sockaddr *) &client_address, &client_address_len);

		// inet_ntoa prints user friendly representation of the
		// ip address
		buffer[len] = '\0';
		printf("received: '%s' from client %s on port %d\n", buffer,
				inet_ntoa(client_address.sin_addr),
				ntohs(client_address.sin_port));

		// send same content back to the client ("echo")

		multiResponse(buffer, sock, client_address);
		cout << "--end multiresponse" << endl;

//		break;
//		string response = generateResponse(buffer);
//		cout << "generated response: " << response << endl;
//		int n = response.length();
//		char char_array[n];
//		strcpy(char_array, response.c_str());
//		int sent_len = sendto(sock, char_array, n + 1, 0,
//				(struct sockaddr *) &client_address, client_address_len);
//		printf("server sent back message of size:%d\n", sent_len);
//		printf("server sent back message containing:%s\n", response);

		//	int pipe1[2]; // Read from parent to check timeout

//		pid_t p;

//		p = fork();

		// Parent process
//		if (p > 0) {
//			int pipe2[2]; // Read from parent to check ACKs
//			pid_t p2;
//			p2 = fork();
//
//			if (p2 > 0) { // parent
//				// read from pipe2 to check ACK
//				// read from pipe1 to check for timeout
//				// take action based on what was read
//			} else { // child2
//				// call recevfrom() to receive the possible ACK from the client
//				// use pipe2 to send the notification to the parent process
//			}
//
//		} else { //child process
//			// start timer
//			// use pipe1 to send notification after timer is done
//		}

	}
	close(sock);
	return 0;
}

