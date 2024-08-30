#include "headers.h"

int cd(char **command, char *home, char *cdstore, int j)
{
    char current[1000];
    getcwd(current, 1000);
    if (j >= 3)
    {
        printf("\033[0;31mToo Many Arguments\033[0;37m\n");
    }
    else if (j == 1)
    {
        chdir(home);
    }
    else if (j == 2)
    {
        int ret;
        if (strcmp(command[1], ".") == 0)
            ret = chdir(".");
        else if (strcmp(command[1], "..") == 0)
            ret = chdir("..");
        else if (strcmp(command[1], "-") == 0)
            ret = chdir(cdstore);
        else if (strcmp(command[1], "~") == 0)
            ret = chdir(home);
        else
            ret = chdir(command[1]);
        if (ret == -1)
            printf("\033[0;31mcd: %s: No such file or directory\033[0;37m\n", command[1]);
        strcpy(cdstore, current);
    }
    return j;
}