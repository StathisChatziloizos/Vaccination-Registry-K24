#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFFER_SIZE 512

char* fifo = "simpleFifo";

int main(int argc, char** argv)
{
    int fd;
    char msgbuf[BUFFER_SIZE+1];

    if(mkfifo(fifo,0666) == -1)     {perror("mkfifo");    return -1;}
    if((fd = open(fifo, O_RDWR)) < 0 )     {perror("Open fifo");    return -1;}

    while(1)
    {
        if(read(fd, msgbuf, BUFFER_SIZE+1) < 0)     {perror("read");    return -1;}

        printf("\nMessage Received: %s", msgbuf);
        fflush(stdout);
    }
    return 0;
}