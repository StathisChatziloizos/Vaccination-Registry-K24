#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>



#define BUFFER_SIZE 512

char* fifo = "simpleFifo";

int main(int argc, char** argv)
{
    int fd, nwrite;
    char msgbuf[BUFFER_SIZE+1];

    //if(mkfifo(fifo,0666) == -1)     {perror("mkfifo");    return -1;}
    if((fd = open(fifo, O_WRONLY | O_NONBLOCK)) < 0 )     {perror("Open fifo");    return -1;}

    while(1)
    {
        scanf("%s",msgbuf);
        if((nwrite = write(fd,msgbuf, BUFFER_SIZE+1)) == -1)    {perror("write");   return -1;}
    }






    return 0;
}