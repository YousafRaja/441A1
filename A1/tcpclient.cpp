/*
 * tcpclient.c
 *
 *  Created on: Jan 16, 2018
 *      Author: reza
 */

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
#include <strings.h>
#include <netdb.h>
#include <string.h>
#include<iostream>
#include<stdio.h>
using namespace std;

void init_sockaddr (struct sockaddr_in *name,
               string hostname,
               uint16_t port)
{
  struct hostent *hostinfo;

  name->sin_family = AF_INET;
  name->sin_port = htons (port);
  hostinfo = gethostbyname ("yahoo.ca");
  if (hostinfo == NULL)
    {
      cout<< stderr<< "Unknown host %s.\n"<< hostname<<endl;
      exit (EXIT_FAILURE);
    }
  name->sin_addr = *(struct in_addr *) hostinfo->h_addr;
}

int main() {

	/* Address initialization */
	struct sockaddr_in server;
	int MYPORTNUM = 80;
	memset(&server, 0, sizeof(server));

	//server.sin_family = AF_INET;
	//server.sin_port = htons(MYPORTNUM);
	//server.sin_addr.s_addr = htonl(INADDR_ANY);

	string hostnam = "pages.cpsc.ucalgary.ca";
	init_sockaddr(&server, hostnam, MYPORTNUM);



	/* Create the listening socket */
	int sock;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		printf("Error in socket() while creating lstn_sock\n");
		exit(-1);
	}

	/* Connect to TCP server */
	int status;
	status = connect(sock, (struct sockaddr *) &server,
			sizeof(struct sockaddr_in));
	if (status < 0) {
		printf("Error in connect()\n");
		exit(-1);
	} else {
		printf("Connected.\n");
	}

	/* Send data*/
	int count;
	char message[1024] = { "GET /~carey/CPSC441/test1.html HTTP/1.1\r\nHost: www.pages.cpsc.ucalgary.ca\r\n\r\n" };
	count = send(sock, message, sizeof(message), 0);
	if (count < 0) {
		printf("Error in send()\n");
	}
	/* Receive data */
	char rcv_message[1024];
	count = recv(sock, rcv_message, sizeof(rcv_message), 0);
	if (count < 0) {
		printf("Error in recv()\n");
	} else {
		printf("Server: %s\n", rcv_message);
	}

	while (1) {
		char buf[1024];  // or whatever size fits your needs.

	if (fgets(buf, sizeof buf, stdin)) {
		buf[strcspn(buf, "\n")] = '\0';
		// handle the input as you would have from gets
		send(sock, buf, sizeof(message), 0);
	} else {
		// handle end of file
	}
		

		if (strstr(message, "Bye") != NULL) {
			exit(0);
		}
	}

	/* Close the socket */
	close(sock);
	return 0;
}
