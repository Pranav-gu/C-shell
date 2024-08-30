#include "headers.h"

int echo(char **command, int startindex, int endindex)
{
    for (int i = startindex; i <= endindex-1; i++)
    {
        if (i != startindex)
            printf("%s ", command[i]);
    }
    printf("\n");
    return endindex;
}