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
	printf("server sent back message containing:%s\n", response);

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
		for (; i < request.size(); i++) {
			filename += request[i];
		}
		int size = fileManager.getFileSize(filename);
		response = intToString(size);
		reply(response, sock, client_address);
	} else if (type == "OctoGetFullLegs:") {
		int filesize;
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

		int start = 0;
		int octaLegSize = 1111;
		for (int i = 0; i<completedSize; i++){
			response = fileManager.getFileRange(filename, start, octaLegSize)
								+ getSequence(i) + packetID;
			start += octaLegSize;
			reply(response, sock, client_address);

		}

	} else if (type == "OctoGetPartLegs:") {
		int filesize, completedBlocks;
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

		for (; i < request.size(); i++) {
			packetID += request[i];
		}

		int completedSize = (filesize / OCTOBLOCK_SIZE) * OCTOBLOCK_SIZE;
		int remainingSize = filesize - completedSize;
		int partialSize = remainingSize - (remainingSize % 8);
		int octaLegSize = partialSize / 8;
		int start = completedSize;
		for (int i = 0; i < 8; i++) {

			response = fileManager.getFileRange(filename, start, octaLegSize)
					+ getSequence(i) + packetID;
			cout << "server's payload size: " << response.size() << endl;
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
			reply(response, sock, client_address);
		}

		for (;i < 8;i++) {
			response = PADBYTE + getSequence(i) + packetID;
			reply(response, sock, client_address);
		}
	}

}

int main(int argc, char *argv[]) {

	//FileManager fileManager;
	//string fileName = "fake.txt";
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
	// run indefinitely
	while (true) {
		char buffer[500];

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

		pid_t p;

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

