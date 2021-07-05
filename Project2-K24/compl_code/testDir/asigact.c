#include <stdio.h>
#include <signal.h>
// #include <stdlib.h>
#include <unistd.h>

void catchSIGINT(int signo);


int main(int argc, char** argv)
{
    static struct sigaction act;
    act.sa_handler=SIG_IGN;
    sigfillset(&act.sa_mask);

    sigaction(SIGINT, &act, NULL);
    sigaction(SIGTSTP, &act, NULL);

    for(int i=0; i<10; i++)
    {
        printf("Hello(%d)\n",i+1);
        usleep(200000);
    }
    act.sa_handler=SIG_DFL;
    sigaction(SIGINT, &act, NULL);

    for(int i=10; i<20; i++)
    {
        printf("Hello(%d)\n",i+1);
        usleep(200000);
    }

    act.sa_handler=SIG_DFL;
    sigaction(SIGTSTP, &act, NULL);

    for(int i=20; i<30; i++)
    {
        printf("Hello(%d)\n",i+1);
        usleep(200000);
    }

    return 0;
}

void catchSIGINT(int signo)
{
    printf("\tCaught SIGINT(signo: %d)\n",signo);

}