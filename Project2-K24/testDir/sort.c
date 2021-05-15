#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int cmpstringp(const void *p1, const void *p2)
{
    return strcmp(*(const char **) p1, *(const char **) p2);
}
int main(int argc, char *argv[])
{
    char* arr[]= {"stathis", "nick", "soccer", "basketball", "curling", "airplane"};

    qsort(&arr[0], 6, sizeof(char *), cmpstringp);

    for (int j = 0; j < 6; j++)
        puts(arr[j]);
    exit(EXIT_SUCCESS);
}