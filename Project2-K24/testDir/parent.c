#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>


int main(int argc, char** argv)
{
    char* directoryName;
    DIR *dir_ptr;
    struct dirent *dir;

    if(argc == 1)
        directoryName = ".";
    else if(argc == 2)
        directoryName = argv[1];
    else
    {
        printf("Usage: ./program {directory}\n");
        return -1;
    }

    if((dir_ptr = opendir(directoryName)) == NULL)
    {
        perror("Open Dir");
        return -1;
    }

    while((dir = readdir(dir_ptr)) != NULL )
    {
        printf("dir->d_in = %ld\n",dir->d_ino);
        printf("dir->d_name = %s\n",dir->d_name);
        // printf("dir->d_off = %ld\n",dir->d_off);
        // printf("dir->d_reclen =%d\n",dir->d_reclen);
        // printf("dir->d_type =%d\n",dir->d_type);
        printf("--------------------------\n");
    }
    closedir(dir_ptr);
    // int pid = fork();
    // int status;
    // if (pid < 0)
    // {
    //     perror("Fork ");
    // }
    // if (pid == 0)
    // {
    //     char* args[] = {"./child", NULL};
    //     execv(args[0], args);   //exec ./child
    // }
    // if (pid > 0)
    // {
    //     waitpid(pid, &status, 0);

    //     printf("Parent typing . . .\nStatus: %d\n", WIFEXITED(status));
    // }


    return 0;
}