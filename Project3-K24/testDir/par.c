#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>

int main(int argc ,char** argv)
{
    int pid = fork();
    int status;
    if (pid < 0)
    {
        perror("Fork ");
    }
    if (pid == 0)
    {
        char* args[] = {"./chi", "second third" ,NULL};
        execv(args[0], args);   //exec ./child
    }
    if (pid > 0)
    {
        waitpid(pid, &status, 0);

        printf("Parent typing . . .\nStatus: %d\n", WIFEXITED(status));
    }
}