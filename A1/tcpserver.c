/*
 * tcpserver.c
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
#include <netdb.h>

int main() {

	/* Address initialization */
	struct sockaddr_in server;
	int MYPORTNUM = 1234657;
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(MYPORTNUM);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	/* Create the listening socket */
	int lstn_sock;
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

	/* Main Loop for listening */
	while (1) {

		/* Accept a connection */
		int connected_sock;
		connected_sock = accept(lstn_sock, NULL,
		NULL);
		if (connected_sock < 0) {
			printf("Error in accept()\n");
			exit(-1);
		}

		/* Send data*/
		int count;
		char message[1024] = { "For termination send \"Bye\"\n" };
		count = send(connected_sock, message, sizeof(message), 0);
		if (count < 0) {
			printf("Error in send()\n");
		}
		/* Receive data */
		//int count;
		char rcv_message[1024];
		count = recv(connected_sock, rcv_message, sizeof(rcv_message), 0);
		if (count < 0) {
			printf("Error in recv()\n");
		} else {
			printf("Client said: %s\n", rcv_message);
		}
		while (1) {
			count = recv(connected_sock, rcv_message, sizeof(rcv_message), 0);
			if (count < 0) {
				printf("Error in recv()\n");
			} else {
				printf("Client said: %s\n", rcv_message);
			}
			if (strstr(rcv_message, "Bye") != NULL) {
				exit(0);
			}
		}

		//if (rcv_message == "Bye") {
		close(connected_sock);
		exit(0);
		//}

	}
	/* Close the socket */
	close(lstn_sock);
	return 0;
}
