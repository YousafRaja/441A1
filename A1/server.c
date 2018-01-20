#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<string.h>

int main(int argc, char *argv[])
{
	/* Variables */
	int sock;
	char* pch;
	struct sockaddr_in server;
	int mysock;
	char buff[1024];
	char m[1024]={};
	char buff1[1024]={};
	char* s=" back from ";
	int rval;
	int i=0;
	
	/* Create socket */
	sock = socket(AF_INET, SOCK_STREAM, 0);   //TCP/IP Socket
	if (sock<0)//check failure
	{
		perror("failed to create socket");
		exit(1);
	}
	server.sin_family=AF_INET;
	server.sin_addr.s_addr=INADDR_ANY;
	server.sin_port=htons(8001);
	
	/* call bind*/
	if(bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
	 perror("bind failed");
	 exit(1);	
	}

	/* Listen */
	listen(sock, 5);
	/* Accept */
	do{
	mysock=accept(sock, (struct sockaddr *) 0, 0);
		if(mysock == -1)
		{
			perror("accept failed");
		}
		else
		{
			memset(buff,0,sizeof(buff));
			if((rval = recv(mysock, buff, sizeof(buff), 0)) < 0)
				perror("reading stream message error");
			else if (rval==0)
				printf("ending connection\n");
			else
				printf("MSG: %s\n",buff);
			printf("got the message(rval = %d)\n", rval);
			strcpy(m,buff);
			pch=strtok(m," ");
				while (pch!=NULL)
				{
				i++;
				 if(i==2)
					{
						strcat(buff1,s);
						strcat(buff1,pch);
					}
				 else
					{
						printf("token:%s\n",pch);
						strcat(buff1,pch);
					}
				 pch=strtok(NULL," ");
				}
			if(send(mysock,buff1, sizeof(buff1),0)<0)
				printf("server sent failed");
			close(mysock);
			
		}
	}while(1);

	return 0;
}
