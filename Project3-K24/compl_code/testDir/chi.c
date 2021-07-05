#include <stdio.h>

int main(int argc, char** argv)
{

	for (int i = 1; i < argc; i++)	// apo to 11 ksekinane ta paths
    {
        printf("argv[%d] %s\n",i,argv[i]);
    }

    return 0;
}