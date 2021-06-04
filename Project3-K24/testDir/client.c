#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORT 8080

void func(int sock, int port);

int main(int argc, char** argv)
{
    int port = atoi(argv[1]), sock, i;
    // printf("\t\tPort %d\n",port);

    struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr*)&server;
    struct hostent *rem;

    if (( sock = socket ( AF_INET , SOCK_STREAM , 0) ) < 0)     {perror("socket client"); return -1;}

    // if (( rem = gethostbyname ( argv [1]) ) == NULL )     {herror ( "gethostbyname client" ) ; exit (1) ;}

    server.sin_family = AF_INET;    // Internet Domain
    // memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
    server.sin_addr.s_addr = htonl(INADDR_ANY); //inet_addr("127.0.0.1");
    server.sin_port = htons(port);  // Server port

    // Initiate connection
    if ( connect ( sock , serverptr , sizeof ( server ) ) < 0)   {perror("connect client"); return -1;}
    printf ( "Connecting to port % d \n ", port ) ;

    func(sock, port);
    shutdown(sock, SHUT_RDWR);
    // close(sock);
}


void func(int socket, int port)
{
    int writeBytes;
    char buff[100];
    int counter = 0;
    for(int i=0; i < 5; i++)
    {
        counter ++;
        sprintf(buff,"port %d %d",port, counter);
        writeBytes = strlen(buff);
        write(socket , &writeBytes, sizeof(int));
        write(socket , buff, writeBytes);

        if (strcmp(buff, "exit") == 0)
        {
            break;
        }
    }
}