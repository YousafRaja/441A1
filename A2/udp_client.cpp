#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>  // Required for stringstreams
#include <string>
#include "OctoPro.h"
using namespace std;
int BUFFER_SIZE = 8888; //max size of message buffer
int OCTOBLOCK_SIZE = 8888;
int OCTOLEG_SIZE = 1111;
int OCTOMINI_SIZE = 8;
char PADBYTE = '~';

string sendMsg(string msg, int sock, struct sockaddr_in server_address) { //TODO: Modify to include ACK # (start byte index/8)



	// send data
	const char* data_to_send = msg.c_str();
	int len = sendto(sock, data_to_send, strlen(data_to_send), 0,
			(struct sockaddr*) &server_address, sizeof(server_address));

	printf("message has been sent to server\n");

	// received echoed data back
	char buffer[BUFFER_SIZE];
	int recv_bytes = recvfrom(sock, buffer, BUFFER_SIZE, 0, NULL, NULL);
	printf("received bytes = %d\n", recv_bytes);
	buffer[BUFFER_SIZE] = '\0';
	printf("recieved: '%s'\n", buffer);
	return buffer;

}

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


string requestOctoBlock(string fileName, int sock,
		struct sockaddr_in server_address, int blockNumber) {
	string data = "OctoGetBlock:" + fileName + " " + intToString(blockNumber);
	return sendMsg(data, sock, server_address);
}

string requestOctoLeg(string fileName, int sock,
		struct sockaddr_in server_address, int blockNumber, int legNumber) {

	string data = "OctoGetLeg:" + fileName + " " + intToString(blockNumber)
			+ " " + intToString(legNumber);

	return sendMsg(data, sock, server_address);
}

string requestOctoMinis(string fileName, int sock,
		struct sockaddr_in server_address, int blockNumber, int legNumber,
		int miniNumber) {
	string data = "OctoGetMinis:" + fileName + " " + intToString(blockNumber)
			+ " " + intToString(legNumber) + " " + intToString(miniNumber);
	return sendMsg(data, sock, server_address);
}

int getFileLength(string fileName, int sock,
		struct sockaddr_in server_address) {
	// send data
	string data = OctoCheck + fileName;
	string response = sendMsg(data, sock, server_address);
	return stringToInt(response);
}

int getPaddedSize(int partialLeg) {
	while (partialLeg % 8 != 0) {
		partialLeg++;
	}
	return partialLeg;
}

string trimPadding(string data) {
	cout << "bytes to trim" << endl;
	cout << data << endl;
	int i = 0;
	int s = data.size() - 1;
	for (; i < s; i++) {
		if (data[s - i] == PADBYTE) {
			i++;
		} else {
			//i--;
			break;
		}
	}
	return data.substr(0, data.size() - i);
}
//string generateRequest(string fileName, int fileLength){
//
//}

int getOctoblocks(int fileLength) {
	return fileLength / OCTOBLOCK_SIZE;
}

int getOctolegs(int blockLength) {
	return blockLength / OCTOLEG_SIZE;
}

int getOctoMinis(int legLength) {
	return (getPaddedSize(legLength) / OCTOMINI_SIZE);
}

void saveToFile(string filename, string data) {
	ofstream myfile;
	myfile.open(filename.c_str());
	myfile << data;
	myfile.close();
}

#define PORT 8001
int main() {

	const char* server_name = "localhost"; //loopback
	const int server_port = PORT;

	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;

	// creates binary representation of server name
	// and stores it as sin_addr
	//inet_pton: convert IPv4 and IPv6 addresses from text to binary form

	inet_pton(AF_INET, server_name, &server_address.sin_addr);

	server_address.sin_port = htons(server_port);

	// open socket
	int sock;
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("could not create socket\n");
		return 1;
	}
	printf("client socket created\n");

	// get filename
	string filename = "1KB.txt";
	//cout <<"Hello, please enter the name of the file you'd like to retrieve" << endl;
	//cin >> filename;

	string fullResponse = "";
	int remainingSize = getFileLength(filename, sock, server_address);
	int b = getOctoblocks(remainingSize);
	remainingSize -= b * OCTOBLOCK_SIZE;

	int l = getOctolegs(remainingSize);
	remainingSize -= l * OCTOLEG_SIZE;

	int m = getOctoMinis(remainingSize);
	remainingSize = m * OCTOMINI_SIZE - remainingSize; //any remaining amount will be pad bytes

	for (int i = 0; i < b; i++) {
		//request full block OctoGetBlock block#
		fullResponse += requestOctoBlock(filename, sock, server_address, i);
		//wait till recved
	}

	for (int j = 0; j < l; j++) { //new process for each leg
		//OctoGetLeg block# leg#
		fullResponse += requestOctoLeg(filename, sock, server_address, b, j);
		cout << "current full response:" << endl;
		cout << fullResponse << endl;
		//wait till recevd
	}

	if (m) {
		string requestedminis = trimPadding(
				requestOctoMinis(filename, sock, server_address, b, l, m));
		cout << "requested minis:" << endl;
		cout << requestedminis << endl;
		fullResponse += requestedminis;
		//fullResponse += trimPadding(requestOctoMinis(filename, sock, server_address, b, l, m));
	}

	cout << "current full response2:" << endl;
	cout << fullResponse << endl;

	//calculate actual remaining size
	//calculate padded size
	//new process for miniblock
	//make request for padded size
	//create new string of actual size and copy response from  server
	//fullResponse =trimPadding(fullResponse);
	//cout <<"CLIENT RECIEVED: "<<trimPadding(fullResponse)<<endl;
	saveToFile(filename, fullResponse);

// data that will be sent ++to the server
	const char* data_to_send = "Hi Server!!!";

// send data
	int mesLen = 100;
	int len = sendto(sock, data_to_send, strlen(data_to_send), 0,
			(struct sockaddr*) &server_address, sizeof(server_address));
	printf("message has been sent to server\n");
// received echoed data back
	char buffer[500];
	int recv_bytes = recvfrom(sock, buffer, mesLen, 0, NULL, NULL);
	printf("received bytes = %d\n", recv_bytes);
//buffer[mesLen] = '\0';
	printf("recieved: '%s'\n", buffer);

// close the socket
	close(sock);
	return 0;
}

