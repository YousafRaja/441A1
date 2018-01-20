#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<netdb.h>

#define DATA "Hello Server"
int main(int argc, char *argv[])
{
	int sock;
	int rval;
	struct sockaddr_in server;
	struct hostent *hp;
	char buff[1024];
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if( sock <0)
	{
		 perror("socket failed");
		 close(sock);
		 exit(1);
	}
	server.sin_family=AF_INET;
/*
	hp = gethostbyname(argv[1]);
	if (hp == 0)
	{
		perror("gethostbyname failed");
		close(sock);
		exit(1);
	}

	//memcpy(&server.sin_addr, hp->h_addr, hp->h_length);
*/
	server.sin_addr.s_addr=htonl(INADDR_ANY);
	server.sin_port=htons(8001);

	if(connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0)
	{
		 perror("connect failed");
		 close(sock);
		 exit(1);
	}

	if(send(sock, DATA, sizeof(DATA), 0) < 0)
	{
		perror("send failed");
		close(sock);
		exit(1);
	}

	printf("Sent %s\n",DATA);
	memset(buff,0,sizeof(buff));
	if((rval = recv(sock, buff, sizeof(buff), 0)) < 0)
		perror("reading stream message error");
	else if (rval==0)
		printf("ending connection\n");
	else
		printf("Recieved MSG: %s\n",buff);
	close(sock);
	return 0;
}
