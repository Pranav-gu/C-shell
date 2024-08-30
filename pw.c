#include "headers.h"

int pw(char **command, int endindex)
{
    char current[1000];
    getcwd(current, 1000);
    printf("%s\n", current);
    return endindex;
}