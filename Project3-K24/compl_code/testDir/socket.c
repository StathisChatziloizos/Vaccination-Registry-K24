#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 9080
#define NUM_PROCESSES 10

void func(int newsock);

int main(int argc, char** argv)
{
    int num_Monitors = atoi(argv[1]);

    int port[num_Monitors], sock[num_Monitors], newsock[num_Monitors];
    struct sockaddr_in server,client[num_Monitors];
    socklen_t clientlen[num_Monitors];

    struct sockaddr *serverptr[num_Monitors];
    struct sockaddr *clientptr[num_Monitors];

    pid_t pid[num_Monitors];

    
    for (int i = 0; i < num_Monitors; i++)
    {
        port[i] = PORT + i;
        serverptr[i]=(struct sockaddr *)&server;
        clientptr[i]=(struct sockaddr *)&client[i];
        // port[i]=PORT;


        // struct hostent *rem;

        // Create Socket
        if (( sock[i] = socket (AF_INET , SOCK_STREAM , 0) ) < 0)     {perror("socket server");  return -1;}

        memset(&server, 0, sizeof(server));
        server.sin_family = AF_INET;    // Internet Domain
        server.sin_addr.s_addr = htonl(INADDR_ANY);
        server.sin_port = htons(port[i]);  // Port PORT 8080

        if (setsockopt(sock[i], SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &(int){1}, sizeof(int)) < 0)     {perror("setsockopt"); return -1;}

        // Bind socket to address
        if ( bind ( sock[i] , serverptr[i] , sizeof ( server ) ) < 0)     {perror("bind server");  return -1;}

        // Listen for connections
        if ( listen ( sock[i] , 50) < 0)    {perror("listen server");  return -1;};
        

        // printf("Listening for connections to port %d\n", port[i]);
        char port_arg[5];
        sprintf(port_arg,"%d",port[i]);

        int number_of_paths = 5;
        char** args2 = (char**)malloc((number_of_paths+3) * sizeof(char*));

        for (int i = 0; i < number_of_paths+3; i++)
        {
            args2[i] = (char*)malloc(20 * sizeof(char));
        }
        

        args2[0] = "./client"; 
        strcpy(args2[1], port_arg); 
        for (int i = 2; i < number_of_paths+2; i++)
        {
            strcpy(args2[i], "Stathis");
        }
        // args2[number_of_paths+2] = NULL;
        


        pid[i] = fork();
        if (pid[i] < 0)     {perror("Fork ");    return -1;}
        if (pid[i] == 0)
        {
            char* args[] = {"./client",port_arg, NULL};
            execv(args2[0], args2);   //exec ./client
            return -1;
        }
        printf("Here\n");
    }
    for (int i = 0; i < num_Monitors; i++)
    {
        clientlen[i] = sizeof(client);

        if (( newsock[i] = accept ( sock[i] , clientptr[i] , & clientlen[i] ) ) < 0)    {perror("accept server");  return -1;}
        // printf("Accepted connection\n");
        close(sock[i]);
        func(newsock[i]);

    }

    for (int i = 0; i < num_Monitors; i++)
    {
        // printf("sock[%d] =  %d\n",i, sock[i]);
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