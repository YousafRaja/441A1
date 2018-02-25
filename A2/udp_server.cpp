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

string generateResponse(string request) {

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
	} else if (type == "OctoGetBlock:") {
		for (; request[i] != ' '; i++) {
			filename += request[i];
		}
		i++;
		for (; i < request.size(); i++) {
			bN += request[i];
			blockNumber = stringToInt(bN);
		}

		response = fileManager.getFileRange(filename,
						(blockNumber),
						blockSize);

	} else if (type == "OctoGetLeg:") {
		for (; request[i] != ' '; i++) {
			filename += request[i];
		}
		i++;
		for (; request[i] != ' '; i++) {
			bN += request[i];
			blockNumber = stringToInt(bN);
		}
		for (; i < request.size(); i++) {
			lN += request[i];
			legNumber = stringToInt(lN);
		}

		response = fileManager.getFileRange(filename,
						((blockNumber*blockSize) + (legNumber*legSize)),
						legSize);

	} else if (type == "OctoGetMinis:") {

		for (; request[i] != ' '; i++) {
			filename += request[i];
		}
		i++;//skip past ' '
		for (; request[i] != ' '; i++) {
			bN += request[i];
			blockNumber = stringToInt(bN);
		}
		i++;
		for (; request[i] != ' '; i++) {
			lN += request[i];
			legNumber = stringToInt(lN);

		}
		for (; i < request.size(); i++) {
			mN += request[i];
			miniNumber = stringToInt(mN);
		}

		response = fileManager.getFileRange(filename,
						((blockNumber*blockSize) + (legNumber*legSize)),
						(miniNumber*miniSize));

	}

	return response;

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
		string response = generateResponse(buffer);
		cout << "generated response: " << response << endl;
		int n = response.length();
		char char_array[n + 1];
		strcpy(char_array, response.c_str());
		int sent_len = sendto(sock, char_array, n + 1, 0,
				(struct sockaddr *) &client_address, client_address_len);
		printf("server sent back message of size:%d\n", sent_len);
		printf("server sent back message containing:%s\n", response);

	}
	close(sock);
	return 0;
}

