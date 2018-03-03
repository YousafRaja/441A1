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
#include <unordered_map>
#include <vector>
#include <map>
#include <random>
#include <sys/time.h>

using namespace std;
int BUFFER_SIZE = 8888; //max size of message buffer
int OCTOBLOCK_SIZE = 8888;
int OCTOLEG_SIZE = 1111;
int OCTOMINI_SIZE = 8;
char PADBYTE = '~';

bool packetDropped = false; //set to true to drop first packet

int getRandom() {
	std::random_device rd;
	std::mt19937 mt(rd()); // seed the Mersenne Twister generator
	std::uniform_int_distribution<> dist(1000, 9999); // define the range
	return dist(mt);
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

void SendMsg(string msg, int sock, struct sockaddr_in server_address) {
	// send data
	const char* data_to_send = msg.c_str();
	int len = sendto(sock, data_to_send, strlen(data_to_send), 0,
			(struct sockaddr*) &server_address, sizeof(server_address));

	printf("message has been sent to server\n");

}

string SendRecvMsg(string msg, int sock, struct sockaddr_in server_address) { //TODO: Modify to include ACK # (start byte index/8)

	// send data
//	const char* data_to_send = msg.c_str();
//	int len = sendto(sock, data_to_send, strlen(data_to_send), 0,
//			(struct sockaddr*) &server_address, sizeof(server_address));
//
//	printf("message has been sent to server\n");
	SendMsg(msg, sock, server_address);

	// received echoed data back
	char buffer[BUFFER_SIZE];
	int recv_bytes = recvfrom(sock, buffer, BUFFER_SIZE, 0, NULL, NULL);
	printf("received bytes = %d\n", recv_bytes);
	buffer[BUFFER_SIZE] = '\0';
	printf("recieved: '%s'\n", buffer);
	return buffer;

}

string extractData(string payload) {
	int s = payload.size();
	return payload.substr(0, s - (8 + 4)); //sequence code 8, id code 4
}

string extractSequence(string payload) {
	int s = payload.size();
	string n = payload.substr(s - 1 - 11, 8);
	return n;
	//return stringToInt(n);
}

int getSequenceNum(string sequence) {
	for (int i = 0; i < sequence.size(); i++) {
		if (sequence[i] == '1') {
			return i;
		}
	}
}

int extractID(string payload) {
	int s = payload.size();
	string n = payload.substr(s - 1 - 3, 4);
	return stringToInt(n);
}

//string sendTinyBlockMsg(string filename, int sock,
//		struct sockaddr_in server_address, int remainingSize, int fileSize) {
//
//	int endPoint = fileSize - 1;
//
//	for (int i = remainingSize; i >= 0; i--) {
//		string msg = "OctoGetTiny:" + filename + " "
//				+ intToString(endPoint - i);
//
//		const char* data_to_send = msg.c_str();
//		int len = sendto(sock, data_to_send, strlen(data_to_send), 0,
//				(struct sockaddr*) &server_address, sizeof(server_address));
//
//		printf("message has been sent to server\n");
//
//	}
//
//	for (int i = 0; i < (8 - remainingSize); i++) {
//		string msg = "OctoGetTiny:" + filename + " " + intToString(-1);
//
//		const char* data_to_send = msg.c_str();
//		int len = sendto(sock, data_to_send, strlen(data_to_send), 0,
//				(struct sockaddr*) &server_address, sizeof(server_address));
//
//		printf("message has been sent to server\n");
//	}
//
//}

string collectResponses(int sock, struct sockaddr_in server_address,
		int expectedID, int parts) { //TODO: Implement
	map<int, string> fileParts;
	bool complete = false;
	char buffer[BUFFER_SIZE];
	bool valid = false;
	while (!complete) {
		int recv_bytes = recvfrom(sock, buffer, BUFFER_SIZE, 0, NULL, NULL);
		if (recv_bytes >= 0) {
			printf("received bytes = %d\n", recv_bytes);
			buffer[BUFFER_SIZE] = '\0';
			printf("recieved payload: '%s'\n", buffer);
			int extractedID = extractID(buffer);
			if (extractedID == expectedID) {
				//extract sequence number
				string seq = "";
				seq = extractSequence(buffer);
				int s = getSequenceNum(seq);
				//process sequence number into map
				fileParts[s] = extractData(buffer);
				if (!packetDropped) {
					cout << "PACKET DROPPED" << endl;
					fileParts.erase(s);
					packetDropped=true;
				}


				//check if complete
				complete = true;
				cout << "recieved part " << intToString(s) << endl;
				for (int i = 0; i < 8; i++) {
					if (!fileParts.count(i)) {
						cout << "still missing part " << i << endl;
						complete = false;
						break; // no need to check further
					} else {

					}
				}
			} else {
				cout << "invalid ID" << endl;
				cout << "extracted:" << extractedID << " expected "
						<< expectedID << endl;
				return "";
			}
		} else {
			//Message Receive Timeout or other error
			return "";
		}
	}

	cout << "all parts received, sending ACK" << endl;
	//received all parts, now sending ack
	int ACK = expectedID;
	SendMsg(intToString(ACK + 1), sock, server_address);
	//concatenate fileParts and returns
	string response = "";
	string temp = "~";
	for (int i = 0; i < 8; i++) {
		if (fileParts[i] != temp) {
			response += fileParts[i];
		}
	}

	return response;
}

string getTinyLegs(string filename, int filesize, int sock,
		struct sockaddr_in server_address) {

	int completedSize = (filesize / OCTOBLOCK_SIZE) * OCTOBLOCK_SIZE;
	int remainingSize = filesize - completedSize;
	int partialSize = remainingSize - (remainingSize % 8);
	int parts = 8;

	if (remainingSize % 8 == 0) { //when no legs remain
		return "";
	}

	string response = "";

	int ranNum = getRandom();

	string msg = "OctoGetTinyLegs:" + filename + " " + intToString(filesize)
			+ " " + intToString(ranNum);

	const char* data_to_send = msg.c_str();
	int len = sendto(sock, data_to_send, strlen(data_to_send), 0,
			(struct sockaddr*) &server_address, sizeof(server_address));

	while (response == "") {

		printf("message has been sent to server\n");

		response = collectResponses(sock, server_address, ranNum, parts);
		if (response == "") {
			printf("Timeout or error in t, re-sending\n");
		}
	}

	return response;

}

string getPartialLegs(string filename, int filesize, int sock,
		struct sockaddr_in server_address) {
	int completedSize = (filesize / OCTOBLOCK_SIZE) * OCTOBLOCK_SIZE;
	int partialSize = filesize - completedSize;
	if (partialSize <= 8) { //when full legs not possible
		return "";
	}
	int parts = 8;
	string response = "";

	int packetID = getRandom();
	string msg = "OctoGetPartLegs:" + filename + " " + intToString(filesize)
			+ " " + intToString(packetID);

	const char* data_to_send = msg.c_str();
	int len = sendto(sock, data_to_send, strlen(data_to_send), 0,
			(struct sockaddr*) &server_address, sizeof(server_address));
	printf("message has been sent to server with the following ID:\n");
			cout << "expectedID:" << packetID << endl;

	while (response == "") {


		response = collectResponses(sock, server_address, packetID, parts);
		if (response == "") {
			printf("Timeout or error in partial legs, waiting for retransmission\n");
		}

	}
	return response;

}

string getFullLegs(string filename, int filesize, int sock,
		struct sockaddr_in server_address) {

	if (filesize < 8888) { //when full legs not possible
		return "";
	}

	int completedBlocks = (filesize / OCTOBLOCK_SIZE);
	int parts = 8;
	int totalParts = completedBlocks * 8;
	string totalResponse = "";
	string response = "";

	int packetID = getRandom();

	string msg = "OctoGetFullLegs:" + filename + " " + intToString(filesize)
			+ " " + intToString(packetID);

	const char* data_to_send = msg.c_str();
	int len = sendto(sock, data_to_send, strlen(data_to_send), 0,
			(struct sockaddr*) &server_address, sizeof(server_address));
	printf("message has been sent to server\n");

	while (totalParts > 0 || response == "") {

		response = collectResponses(sock, server_address, packetID, totalParts);
		if (response == "") {
			printf("Timeout or error in full legs, re-sending\n");
		} else {
			packetID++; //update after every full block is received
			totalParts -= 8;
			//cout "RECEIVED OCTOBLOCK:"<<endl;
			//cout<<
			totalResponse += response;
		}

	}

	return totalResponse;

}

//string sendPartialBlockMsg(string filename, int sock,
//		struct sockaddr_in server_address, int blockNumber, int remainingSize) {
//
//	int partialSize = remainingSize / 8;
//	for (int i = 0; i < 8; i++) {
//		int start = i * partialSize;
//		int end = start + partialSize;
//		string msg = "OctoGetLegPartial:" + filename + " "
//				+ intToString(blockNumber) + " " + intToString(start) + " "
//				+ intToString(end);
//
//		const char* data_to_send = msg.c_str();
//		int len = sendto(sock, data_to_send, strlen(data_to_send), 0,
//				(struct sockaddr*) &server_address, sizeof(server_address));
//
//		printf("message has been sent to server\n");
//
//	}
//
//	return collectResponses(sock);
//
//}

//string sendBlockMsg(string filename, int sock,
//		struct sockaddr_in server_address, int blockNumber) {
//// used to get 8 full legs
//
//	for (int i = 0; i < 8; i++) {
//		string msg = "OctoGetLeg:" + filename + " " + intToString(blockNumber)
//				+ " " + intToString(i);
//
//		const char* data_to_send = msg.c_str();
//		int len = sendto(sock, data_to_send, strlen(data_to_send), 0,
//				(struct sockaddr*) &server_address, sizeof(server_address));
//
//		printf("message has been sent to server\n");
//
//	}
//
//	return collectResponses(sock);
//}

string requestOctoBlock(string fileName, int sock,
		struct sockaddr_in server_address, int blockNumber) {
	string data = "OctoGetBlock:" + fileName + " " + intToString(blockNumber);
	return SendRecvMsg(data, sock, server_address);
}

string requestOctoLeg(string fileName, int sock,
		struct sockaddr_in server_address, int blockNumber, int legNumber) {

	string data = "OctoGetLeg:" + fileName + " " + intToString(blockNumber)
			+ " " + intToString(legNumber);

	return SendRecvMsg(data, sock, server_address);
}

string requestOctoMinis(string fileName, int sock,
		struct sockaddr_in server_address, int blockNumber, int legNumber,
		int miniNumber) {
	string data = "OctoGetMinis:" + fileName + " " + intToString(blockNumber)
			+ " " + intToString(legNumber) + " " + intToString(miniNumber);
	return SendRecvMsg(data, sock, server_address);
}

int getFileLength(string fileName, int sock,
		struct sockaddr_in server_address) {
	// send data
	int ACK = getRandom();
	string data = OctoCheck + fileName + " " + intToString(ACK);
	string response = SendRecvMsg(data, sock, server_address);
	int len = response.length();
	response = response.substr(0, len - 4);
	SendMsg(intToString(ACK + 1), sock, server_address);
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

	struct timeval timeout = { 60, 0 }; //set timeout for 2 seconds

	/* set receive UDP message timeout */

	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*) &timeout,
			sizeof(struct timeval));

	// get filename
	string filename = "256KB.txt";
	//cout <<"Hello, please enter the name of the file you'd like to retrieve" << endl;
	//cin >> filename;

	string fullResponse = "";
	int remainingSize = getFileLength(filename, sock, server_address);
	int b = getOctoblocks(remainingSize);
	int partialBlock;
	int filesize = remainingSize;
	fullResponse += getFullLegs(filename, filesize, sock, server_address);
	cout << "Fullleg:" << endl;
	cout << fullResponse << endl;
	string partialResponse = getPartialLegs(filename, filesize, sock,
			server_address);
	cout << "Partialleg:" << endl;
	cout << partialResponse << endl;
	fullResponse += partialResponse;

	string TinyLeg = getTinyLegs(filename, filesize, sock, server_address);
	cout << "TinyLeg:" << endl;
	cout << TinyLeg << endl;
	fullResponse += TinyLeg;
	//remainingSize -= (filesize / OCTOBLOCK_SIZE) * OCTOBLOCK_SIZE;
	//partialBlock = remainingSize - (remainingSize % 8);
	//remainingSize = (remainingSize % 8);

	//remainingSize -= b * OCTOBLOCK_SIZE;

	//int l = getOctolegs(remainingSize);
	//remainingSize -= l * OCTOLEG_SIZE;

	//int m = getOctoMinis(remainingSize);
	//remainingSize = m * OCTOMINI_SIZE - remainingSize; //any remaining amount will be pad bytes

//	int i = 0;
//	while (remainingSize >= 8888) {
//		// get all 8 parts of the full octoblock
//		fullResponse += sendBlockMsg(filename, sock, server_address, i);
//		i++;
//		remainingSize -= 8888;
//	}
//
//	int partialBlock = 0;
//	if (remainingSize > 8) {
//		partialBlock = remainingSize - (remainingSize % 8);
//		fullResponse += sendPartialBlockMsg(filename, sock, server_address, i,
//				partialBlock);
//	}
//	remainingSize -= partialBlock;
//	if (remainingSize > 0) {
//		fullResponse += sendTinyBlockMsg(filename, sock, server_address,
//				remainingSize, filesize);
//	}

	cout << "current full response2:" << endl;
	cout << fullResponse << endl;

	saveToFile(filename, fullResponse);

// close the socket
	close(sock);
	return 0;
}
