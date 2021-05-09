#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void f();


int main(int argc, char** argv)
{
    signal(SIGINT,f);
    while (1)
    {
        usleep(500000); // 0.5sec
        printf("Hello!\n");
    }
    
    return 0;
}

void f()
{
    signal(SIGINT, f);
    printf("\tSIGINT handler\n");
}