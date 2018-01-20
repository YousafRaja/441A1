#include<stdio.h>
#include<sys/socket.h> 
#include <iostream>


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

using namespace std;

sockaddr_in startSERVER(){
	struct sockaddr_in server;
	
	int MYPORTNUM = 1234657;
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(MYPORTNUM);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	
	return server;
}
 
int main(int argc , char *argv[])
{
    int socket_desc;
    
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
     
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    
    
    
    cout<<"Exiting"<<endl;
     
    return 0;
}
