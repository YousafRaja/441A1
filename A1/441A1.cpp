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

using namespace std;
void extractRequest (char rcv_message [], string &hostname, string &reqMsg);
void init_sockaddr (struct sockaddr_in *name,
               string hostname,
               uint16_t port);

void bindSocket(sockaddr_in &server, int &lstn_sock){
	/* Bind a socket to a server that already has an address and port */
	/* Bind the socket to address and port */
		int status;
		status = bind(lstn_sock, (struct sockaddr *) &server,
				sizeof(struct sockaddr_in));
		if (status < 0) {
			printf("Error in bind()\n");
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

string proxyClient(char rcv_message []){
	string hostname = "", reqMsg = "";
	extractRequest(rcv_message, hostname, reqMsg);

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

	/* Send data*/
		int count;
		char message[1024];
		strcpy(message, reqMsg.c_str());
		count = send(sock, message, sizeof(message), 0);
		if (count < 0) {
			printf("Error in send()\n");
		}


		/* Receive data */
			char ws_message[1024];
			count = recv(sock, ws_message, sizeof(ws_message), 0);
			if (count < 0) {
				printf("Error in recv()\n");
			} else {
				printf("Server: %s\n", ws_message);
			}


			using namespace boost::algorithm;
				string str(ws_message);


		int i =str.find("<html");


		string test = str.substr(i, str.length()-1);

		return str.substr(i, str.length()-1);

}


void init_sockaddr (struct sockaddr_in *name,
               string hostname,
               uint16_t port)
{
  struct hostent *hostinfo;

  name->sin_family = AF_INET;
  name->sin_port = htons (port);
  hostinfo = gethostbyname ("pages.cpsc.ucalgary.ca");// works only sometimes
  if (hostinfo == NULL)
    {
      cout<< stderr<< "Unknown host %s.\n"<< hostname<<endl;
      exit (EXIT_FAILURE);
    }
  name->sin_addr = *(struct in_addr *) hostinfo->h_addr;
}




void extractRequest (char rcv_message [], string &hostname, string &reqMsg){
	using namespace boost::algorithm;
	string str(rcv_message);
	//msg = "GET http://pages.cpsc.ucalgary.ca/~carey/CPSC441/test1.html HTTP/1.1";
	 std::vector<std::string> tokens;
	 split(tokens, str, is_any_of(" "));

	 string type = tokens[0];

	 unsigned int i = 7;
	 for (; i<tokens[1].length(); i++){
		 if (tokens[1][i]=='/'){ break;}
		 hostname += tokens[1][i];
	 }

	 string node = "";
	 for (; i<tokens[1].length(); i++){
		 node += tokens[1][i];
	 }

	 string httpHost = tokens[2];

	 if (type=="GET"){
		 reqMsg = "GET " + node + " " + httpHost + " " + hostname + "\r\n\r\n";
	 }





}




int main(int argc, char *argv[]) {
// How to differentiate client socket and server socket
// How to get ip address? Parse info in GET request?
	cout <<"Starting ProxyServerSetup"<<endl;
	struct sockaddr_in server;
	int lstn_sock;
	int portNumber=12388;
	setupProxyServer(server, lstn_sock, portNumber);

	/* Main Loop for listening */
	while(1){

		/* Accept a connection */
		int connected_sock;
		connected_sock = accept(lstn_sock, NULL, NULL);
		if (connected_sock < 0) {
			printf("Error in accept()\n");
			exit(-1);
		} else {
			cout <<"a client has connected"<<endl;
		}

		/* Create the proxy listening socket */
		//int proxy_sock;
		//proxy_sock = socket(AF_INET, SOCK_STREAM, 0);
		//if (proxy_sock < 0) {
		//	printf("Error in socket() while creating prxy_sock\n");
		//	exit(-1);
		//}

		/* Send data*/
		int count;
		//char message[1024] = { "For termination send \"Bye\"\n" };
		//count = send(connected_sock, message, sizeof(message), 0);
		//if (count < 0) {
		//	printf("Error in send()\n");
		//}

		/* Receive data */
		char rcv_message[1024];
		count = recv(connected_sock, rcv_message, sizeof(rcv_message), 0);
		if (count < 0) {
			printf("Error in recv()\n");
		} else {
			string wsMsg =  proxyClient(rcv_message);
			char ws_message[wsMsg.length()];
			strcpy(ws_message, wsMsg.c_str());
			count = send(connected_sock, ws_message, sizeof(ws_message),0);
			close(connected_sock);
			printf("Client said: %s\n", rcv_message);
			printf("WS response through proxy said: %s\n", ws_message);
		}



	}


	cout << "Exiting" << endl;

	return 0;
}
