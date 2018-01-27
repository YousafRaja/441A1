#include<stdio.h>
#include<sys/socket.h> 
#include <iostream>

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <string.h>
#include <vector>
#include <ctype.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <netdb.h>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <iostream>
#include <sstream>

using namespace std;
void extractRequest(char rcv_message[], string &hostname, string &reqMsg,
		int &byteRange, bool ranged);
void init_sockaddr(struct sockaddr_in *name, string hostname, uint16_t port);
void init_socket(sockaddr_in &server, int &lstn_sock);

const int BYTE_WIDTH = 100;
const int MESSAGE_BUFFER = 100000;

void init_socket(sockaddr_in &server, int &lstn_sock) {
	/* Bind a new socket to a server that already has an address and port */

	/* Create the listening socket */
	lstn_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (lstn_sock < 0) {
		printf("Error in socket() while creating lstn_sock\n");
		exit(-1);
	}

	/* Bind the socket to address and port */
	int status;
	status = bind(lstn_sock, (struct sockaddr *) &server,
			sizeof(struct sockaddr_in));
	if (status < 0) {
		printf("Error in bind()\n");
		exit(-1);
	}

	/* Connect to TCP server */
	status = listen(lstn_sock, 5);
	if (status < 0) {
		printf("Error in listen()\n");
		exit(-1);
	}

}
//use ip addr show:
void setupProxyServer(sockaddr_in &server, int &lstn_sock, int portNumber) {

	/* Address initialization */
	int MYPORTNUM = portNumber;
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(MYPORTNUM);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	init_socket(server, lstn_sock);

}

string getBody(string ws_message){
	string str = ws_message;
	int i = str.find("\r\n\r\n");
	string body = str.substr(i+4, BYTE_WIDTH);
	cout <<"GETBODY: "<< body << endl;
	return body;
}

int getResponseLength(string ws_message) {
	int c = ws_message.find("Content-Length: ") + 16;
	string l = "";
	while (ws_message[c] != 13) {
		l += ws_message[c];
		c++;
	}

	stringstream ss(l);
	int r;
	ss >> r;
	return r;
}

string webServerCall(sockaddr_in webServer, string reqMsg) {

	/* Create the listening socket */
	int sock;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		printf("Error in socket() while creating lstn_sock\n");
		exit(-1);
	}

	/* Connect to TCP server */
	int status;
	status = connect(sock, (struct sockaddr *) &webServer,
			sizeof(struct sockaddr_in));
	if (status < 0) {
		printf("Error in connect()\n");
		exit(-1);
	} else {
		printf("Connected.\n");
	}

	/* Send data to Web Server*/
	int count;
	char message[MESSAGE_BUFFER];
	strcpy(message, reqMsg.c_str());
	count = send(sock, message, sizeof(message), 0);
	if (count < 0) {
		printf("Error in send()\n");
	}

	/* Receive data from Web Server*/
	char ws_message[MESSAGE_BUFFER];
	count = recv(sock, ws_message, sizeof(ws_message), 0);
	if (count < 0) {
		printf("Error in recv()\n");
	} else {
		cout << ("Server: %s\n", ws_message);
	}

	close(sock);
	return ws_message;
}

string proxyClient(char rcv_message[]) { //todo:pass in upper byte limit,
	string hostname = "", reqMsg = "";
	int byteRange = 0;
	extractRequest(rcv_message, hostname, reqMsg, byteRange, false);

	struct sockaddr_in webServer;
	int PORTNUM = 80;
	memset(&webServer, 0, sizeof(webServer));

	init_sockaddr(&webServer, hostname, PORTNUM);

	int byteLimit = getResponseLength(webServerCall(webServer, reqMsg));

	string str;
	while (byteRange <= byteLimit) {

//		/* Send data to Web Server*/
//		int count;
//		char message[1024];
//		strcpy(message, reqMsg.c_str());
//		count = send(sock, message, sizeof(message), 0);
//		if (count < 0) {
//			printf("Error in send()\n");
//		}
//
//		/* Receive data from Web Server*/
//		char ws_message[1024];
//		count = recv(sock, ws_message, sizeof(ws_message), 0);
//		if (count < 0) {
//			printf("Error in recv()\n");
//		} else {
//			printf("Server: %s\n", ws_message);
//		}
		string tempMsg = "";
		string tempHost = "";
		extractRequest(rcv_message, tempHost, tempMsg, byteRange, true);
		str += getBody(webServerCall(webServer, tempMsg));
		byteRange += BYTE_WIDTH;

	}

	using namespace boost::algorithm;

	int i = str.find("<html>");
	int j = str.find("</html>");
	if (j<0){
		cout <<"Error: Missing </html> tag, check message buffer size" << endl;
	}
	return str.substr(i, (j-i)+7);

}

void init_sockaddr(struct sockaddr_in *name, string hostname, uint16_t port) {
	struct hostent *hostinfo;

	name->sin_family = AF_INET;
	name->sin_port = htons(port);
	hostinfo = gethostbyname("pages.cpsc.ucalgary.ca"); // works only sometimes
	if (hostinfo == NULL) {
		cout << stderr << "Unknown host %s.\n" << hostname << endl;
		exit(EXIT_FAILURE);
	}
	name->sin_addr = *(struct in_addr *) hostinfo->h_addr;
}

void extractRequest(char rcv_message[], string &hostname, string &reqMsg,
		int &byteRange, bool ranged) {
	using namespace boost::algorithm;
	string str(rcv_message);
	//msg = "GET http://pages.cpsc.ucalgary.ca/~carey/CPSC441/test1.html HTTP/1.1";
	std::vector<std::string> tokens;
	split(tokens, str, is_any_of(" "));

	string type = tokens[0];

	unsigned int i = 7;
	for (; i < tokens[1].length(); i++) {
		if (tokens[1][i] == '/') {
			break;
		}
		hostname += tokens[1][i];
	}

	string node = "";
	for (; i < tokens[1].length(); i++) {
		node += tokens[1][i];
	}

	string httpHost = tokens[2];

	if (type == "GET") {
		reqMsg = type + " " + node + " " + httpHost + " " + hostname + "\r\n";

		if (ranged) {
			std::stringstream ss;
			ss << byteRange;
			string from, to;
			ss >> from;
			ss.clear();
			ss << (byteRange + BYTE_WIDTH);
			ss >> to;

			/*ranged request*/
			reqMsg += "Range: bytes=" + from + "-" + to + "\r\n";
		}
	}

	/*final step*/
	reqMsg += "\r\n";

}

int main(int argc, char *argv[]) {

// Should buffer size depend on the page?
	cout << "Starting ProxyServerSetup" << endl;
	struct sockaddr_in server;
	int lstn_sock;
	int portNumber = 12284;
	setupProxyServer(server, lstn_sock, portNumber);

	/* Main Loop for listening */
	while (1) {

		/* Accept a connection */
		int connected_sock;
		connected_sock = accept(lstn_sock, NULL, NULL);
		if (connected_sock < 0) {
			printf("Error in accept()\n");
			exit(-1);
		} else {
			cout << "a client has connected" << endl;
		}

		/* Send data*/
		int count;

		/* Receive data from browser*/
		char rcv_message[MESSAGE_BUFFER];
		count = recv(connected_sock, rcv_message, sizeof(rcv_message), 0);
		if (count < 0) {
			printf("Error in recv()\n");
		} else {
			/* Receive data from proxy client */
			string wsMsg = proxyClient(rcv_message); //todo:pass in a requested byte size
			char ws_message[wsMsg.length()];
			strcpy(ws_message, wsMsg.c_str());
			count = send(connected_sock, ws_message, sizeof(ws_message), 0);
			close(connected_sock);
			printf("Client said: %s\n", rcv_message);
			printf("WS response through proxy said: %s\n", ws_message);
		}

	}

	cout << "Exiting" << endl;

	return 0;
}
