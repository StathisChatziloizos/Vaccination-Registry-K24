#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080
#define NUM_PROCESSES 8

void func(int newsock);

int main(int argc, char** argv)
{
    int port[NUM_PROCESSES], sock[NUM_PROCESSES], newsock[NUM_PROCESSES];
    struct sockaddr_in server[NUM_PROCESSES],client[NUM_PROCESSES];
    socklen_t clientlen[NUM_PROCESSES];

    struct sockaddr *serverptr[NUM_PROCESSES];
    struct sockaddr *clientptr[NUM_PROCESSES];

    pid_t pid[NUM_PROCESSES];

    
    for (int i = 0; i < NUM_PROCESSES; i++)
    {
        port[i] = PORT + i;
        serverptr[i]=(struct sockaddr *)&server[i];
        clientptr[i]=(struct sockaddr *)&server[i];
        // port[i]=PORT;


        // struct hostent *rem;

        // Create Socket
        if (( sock[i] = socket (AF_INET , SOCK_STREAM , 0) ) < 0)     {perror("socket server");  return -1;}
        printf("sock[%d] =  %d\n",i, sock[i]);
        fflush(stdout);

        // memset(&server[i], 0, sizeof(server));
        server[i].sin_family = AF_INET;    // Internet Domain
        server[i].sin_addr.s_addr = htonl(INADDR_ANY);
        server[i].sin_port = htons(port[i]);  // Port PORT 8080

        // printf("ptr = %d, %d",server[i].sin_port, serverptr[i]->sa_family)

        if (setsockopt(sock[i], SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)     {perror("setsockopt"); return -1;}

        // Bind socket to address
        if ( bind ( sock[i] , serverptr[i] , sizeof ( server ) ) < 0)     {perror("bind server");  return -1;}

        // Listen for connections
        if ( listen ( sock[i] , 50) < 0)    {perror("listen server");  return -1;};
        

        // printf("Listening for connections to port %d\n", port[i]);

        char port_arg[5];
        sprintf(port_arg,"%d",port[i]);

        pid[i] = fork();
        if (pid[i] < 0)     {perror("Fork ");    return -1;}
        if (pid[i] == 0)
        {
            char* args[] = {"./client",port_arg, NULL};
            execv(args[0], args);   //exec ./client
            return -1;
        }
    }
    for (int i = 0; i < NUM_PROCESSES; i++)
    {
        clientlen[i] = sizeof(client);

        if (( newsock[i] = accept ( sock[i] , clientptr[i] , & clientlen[i] ) ) < 0)    {perror("accept server");  return -1;}
        printf("Accepted connection\n");
        close(sock[i]);
        func(newsock[i]);

    }

    return 0;
}

void func(int newsock)
{
    int readBytes;
    char* buff=(char*)malloc(1);
    int nread;
    while (1)
    {
        nread = read(newsock, &readBytes, sizeof(int));
        if (nread <= 0)
        {
            break;
        }
        buff = (char*)realloc(buff, readBytes+1);
        read(newsock, buff, readBytes);
        printf("Client sent: %s\n",buff);

        if (strcmp(buff, "exit") == 0)
        {
            break;
        }
        
    }
    free(buff);
}