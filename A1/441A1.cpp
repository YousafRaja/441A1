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
#include <thread>

using namespace std;
const int PROXYPORT = 12280;

class ProxyNode {

public:

	const int BYTE_WIDTH = 100; // max recommended size: 100000
	const int MESSAGE_BUFFER = 1000000; //max recommended size: 1000000
	static const int num_threads = 100;
	const bool SLOWDOWN = true;


//This function will be called from a thread

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
		status = listen(lstn_sock, 100);
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

	string getBody(string ws_message) {
		string str = ws_message;
		int i = str.find("\r\n\r\n");
		if (i<0){
			cout <<"Error: Header/Body divider not found" <<endl;
		}
		string body = str.substr(i + 4, BYTE_WIDTH);
		cout <<"WS_MESSAGE: " + ws_message << endl;
		cout << "GETBODY: " + body << endl;
		return body;
		//return ws_message;
	}

	void configureByResponse(string &ws_message, int &byteLength, bool &isHtml) {

		cout << "CONFIGURATION MESSAGE: " << ws_message <<endl;

		int c = ws_message.find("Content-Length: ") + 16;
		if (c<=16){ //-1 + 16 (if it's not found, find will return -1, then need we added 16 to it)
			isHtml = false;
			return;
		}
		string l = "";
		while (ws_message[c] != 13) {
			l += ws_message[c];
			c++;
		}

		if (l==""){
			cout <<"Error: no content length found" <<endl;
		}

		stringstream ss(l);
		int r;
		ss >> r;
		byteLength = r;

		c = ws_message.find("Content-Type: ") + 14;
		string t = "";
		while (ws_message[c] != '\r') {
			t += ws_message[c];
			c++;
		}
		int result = t.find("htm");
		isHtml = result >= 0 ? true : false;
	}

	string getStatusCode(string webserverResponse){
		string skip = "HTTP/1.1";
		int j = webserverResponse.find("HTTP/1.1") + skip.length() + 1;
		string code = "";
		for (int i = j; i<webserverResponse.length(); i++){
			if (webserverResponse[i]=='\r' || webserverResponse[i]==' '){
				break;
			}
			code += webserverResponse[i];
		}
		return code;
	}

	string webServerCall(string hostname, string reqMsg, char* msg) {
/*Recieves messages from the proxyClient, sends them to the web server, and returns the response to the proxyClient*/

		struct sockaddr_in webServer;
				int PORTNUM = 80;
				memset(&webServer, 0, sizeof(webServer));

				init_sockaddr(&webServer, hostname, PORTNUM);

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
		strcpy(msg, reqMsg.c_str());
		string a;
		cout <<"MESSAGE BEING SENT TO WEBSERVER: " << msg << endl;
		count = send(sock, msg, sizeof(message), 0);
		if (count < 0) {
			printf("Error in send()\n");
		}

		/* Receive data from Web Server*/
		char ws_message[MESSAGE_BUFFER];
		memset(msg, 0, MESSAGE_BUFFER);
		string test(msg);
		count = recv(sock, msg, sizeof(ws_message), 0);
		if (count < 0) {
			printf("Error in recv()\n");
		} else {
			cout << "WEBSERVER TO PROXY: " << endl;
			cout << msg << endl;
		}

		string code = getStatusCode(msg);

		stringstream ss(code);
		int codeNum = 0;
		ss >> codeNum;

		string errorString;
		/*
		 *
		 *
    200 (OK)
    206 (Partial Content)
    301 (Moved Permanently) (a.k.a. HTTP redirection)
    302 (Found) (a.k.a. HTTP redirection)
    304 (Not Modified)
    404 (Not Found)
		 *
		 *
		 */
		switch(codeNum) {
		    case 200 : cout << "(OK)" <<endl; // prints status code
		             break;       // and exits the switch
		    case 206 : cout << "(Partial Content)"<<endl;
		             break;
		    case 301 : cout << "(Moved Permanently) (a.k.a. HTTP redirection)"<<endl;
		    		 break;
		    case 302 : cout << "(Found) (a.k.a. HTTP redirection)"<<endl;
		    		 break;
		    case 304 : cout << "(Not Modified)"<<endl;
		    		 break;
		    case 404 : cout << "(Not Found)"<<endl;
		    		 break;


		}

		close(sock);
		return msg;
	}

	string proxyClient(char rcv_message[]) {
/*Recieves messages from the browser and sends them to the server using webServerCall, returns the response to the browser*/
		string hostname = "", reqMsg = "";
		int byteRange = 0;
		extractRequest(rcv_message, hostname, reqMsg, byteRange, false);
		char msg[MESSAGE_BUFFER];
		int byteLimit = 0;
		bool isHtml = false;

		string fullResponse = webServerCall(hostname, reqMsg, msg);
		configureByResponse(fullResponse, byteLimit, isHtml); //use fullResponse to configure byteLimit and isHtml
		if (!SLOWDOWN || !isHtml) {
			return fullResponse;
		}


		string partialResponse;
		while (byteRange <= byteLimit) {
			stringstream ss;
			ss << byteRange;
			string br;
			ss >> br;
			cout <<"byteRange: " + br <<endl; // for debug purposes
			string tempMsg = "";
			string tempHost = "";
			extractRequest(rcv_message, tempHost, tempMsg, byteRange, true);
			string resp = webServerCall(hostname, tempMsg, msg);
			partialResponse += getBody(resp);
			byteRange += BYTE_WIDTH;
		}

	return partialResponse; // should contain the full response at this point
}

void init_sockaddr(struct sockaddr_in *name, string hostname, uint16_t port) {
	struct hostent *hostinfo;

	name->sin_family = AF_INET;
	name->sin_port = htons(port);
	auto test = hostname.c_str();
	hostinfo = gethostbyname(hostname.c_str());
	if (hostinfo == NULL) {
		cout << stderr << "Unknown host %s.\n" << hostname << endl;
		exit(EXIT_FAILURE);
	}
	name->sin_addr = *(struct in_addr *) hostinfo->h_addr;
}

void extractRequest(char rcv_message[], string &hostname, string &reqMsg,
		int &byteRange, bool ranged) {
	using namespace boost::algorithm;
	hostname = "";
	string str(rcv_message);
	std::vector<std::string> tokensV;
	split(tokensV, str, is_any_of(" "));

	string type = tokensV[0];

	int ti = 0;
	for (string t : tokensV) { //output everything for debug purposes
		string index;
		std::stringstream ss;
		ss << ti;
		ss >> index;
		cout << index + " " + t << endl;
		ti++;
	}

	unsigned int i = 7; //offset by 7 to skip the "http://"
	for (; i < tokensV[1].length(); i++) {
		if (tokensV[1][i] == '/') {
			break;
		}
		hostname += tokensV[1][i];
	}

	string node = "";
	for (; i < tokensV[1].length(); i++) {
		node += tokensV[1][i];
	}

	string httpHost = tokensV[2]; //should contain HTTP/1.1\r\nHost:

	string extractedHostName = "";
	int j = 0;
	for (; j < tokensV[3].length(); j++) {
		if (tokensV[3][j] == '\r') {
			j += 2; //skip \r and skip the following ]n
			break;
		}
		extractedHostName += tokensV[3][j];
	}
	string nextTag = ""; //next tag after host
	for (; j < tokensV[3].length(); j++) {
		nextTag += tokensV[3][j];
	}

	string remainingRequest = str.substr(str.find(nextTag), str.length());

	int k = remainingRequest.find("Accept-Encoding:");
	if (k >= 0) {
		int m = k + 16; //skip past Accept-Encoding
		string preTag = remainingRequest.substr(0, k);
		for (; m < remainingRequest.length(); m++) {
			if (remainingRequest[m] == '\r') {
				m += 2;
				break;
			}
		}
		string postTag = remainingRequest.substr(m, remainingRequest.length());
		//remainingRequest = preTag + "Accept-Encoding: identity\r\n" + postTag;
		remainingRequest = preTag + postTag;
	}

	reqMsg = type + " " + "http://" + hostname + node + " " + httpHost + " "
			+ extractedHostName + "\r\n";

	int total = 0;
	if (ranged) {
		std::stringstream ss;
		ss << byteRange;
		string from, to;
		ss >> from;
		ss.clear();
		total = byteRange + BYTE_WIDTH;
		ss << total;
		ss >> to;
		ss.clear();

		/*ranged request*/
		reqMsg += "Range: bytes=" + from + "-" + to + "\r\n";
		if(to==""){
			cout <<"Error: string to is not set" <<endl;
		}
	}


	int n = remainingRequest.find("\r\n\r\n") + 4; //+4 to skip over \r\n\r\b
	string remainingRequestEndChecked="";
	for (int i = 0; i<n; i++){
		remainingRequestEndChecked+=remainingRequest[i]; //making sure that remaining request does not go past the end of the header
	}

	/*final step*/
	reqMsg += remainingRequestEndChecked;
	cout << "REQUEST FROM BROWSER: " << endl;
	cout << str << endl;
	cout << "--end--" << endl;
	cout << "REQUEST FROM PROXY: " << endl;
	cout << reqMsg << endl;
	cout << "--end--" << endl;
	//reqMsg = str;
	//reqMsg += "\r\n";

}

void startProxy(int connected_sock) {
	/* Receive data from browser*/
	char rcv_message[MESSAGE_BUFFER];
	int count = recv(connected_sock, rcv_message, sizeof(rcv_message), 0);
	if (count < 0) {
		printf("Error in recv()\n");
	} else {
		/* Receive data from proxy client */
		string wsMsg = proxyClient(rcv_message);
		char ws_message[wsMsg.length()];
		strcpy(ws_message, wsMsg.c_str());
		count = send(connected_sock, ws_message, sizeof(ws_message), 0);
		close(connected_sock);
		printf("Client said: %s\n", rcv_message);
		printf("WS response through proxy said: %s\n", ws_message);
	}
}

};

int main(int argc, char *argv[]) {

cout << "Starting ProxyServerSetup" << endl;
struct sockaddr_in server;
int lstn_sock;
int portNumber = PROXYPORT;
ProxyNode* pN = new ProxyNode();
pN->setupProxyServer(server, lstn_sock, portNumber);

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
		std::thread th(&ProxyNode::startProxy, pN, connected_sock);
		th.join();
	}

}

cout << "Exiting" << endl;
close(lstn_sock);
return 0;
}
