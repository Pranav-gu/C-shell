#include "headers.h"

void find(char *directory, int d_flag, int f_flag, int search)
{
    DIR *d;
    struct dirent *x;
    if (search)
    {
        char current[1000];
        getcwd(current, 1000);
        d = opendir(current);
        while ((x = readdir(d)) != NULL)
        {
            struct stat info;
            char path[2000];
            strcpy(path, current);
            strcat(path, "/");
            strcat(path, x->d_name);
            if (strcmp(x->d_name, directory) == 0)
            {
                printf("%s\n", path);
                return;
            }
            if (S_ISDIR(info.st_mode))
            {
                if (strcmp(x->d_name, "..") == 0 || strcmp(x->d_name, ".") == 0)
                    continue;
                chdir(path);
                find(directory, d_flag, f_flag, search);
                chdir(current);
            }
        }
        return;
    }

    d = opendir(directory);
    if (d == NULL)
    {
        printf("\033[0;31mdiscover: %s: No such file or directory\033[0;37m\n", directory);
        return;
    }

    while ((x = readdir(d)) != NULL)
    {
        struct stat info;
        char path[2000];
        strcpy(path, directory);
        strcat(path, "/");
        strcat(path, x->d_name);
        stat(path, &info);
        if (S_ISDIR(info.st_mode))
        {
            if (d_flag)
                printf("\033[0;34m%s\033[0;37m\n", path);
            if (strcmp(x->d_name, "..") == 0 || strcmp(x->d_name, ".") == 0)
                continue;
            find(path, d_flag, f_flag, search);
        }
        if (f_flag)
            printf("%s\n", path);
    }
    return;
}


int check_absolute_path(char *command)
{
    char current[1000];
    getcwd(current, 1000);
    char parent[1000];
    const char* lastSlash = strrchr(command, '/');
    if (lastSlash == NULL)
        return 0;
    size_t parentDirLength = lastSlash - command;
    strncpy(parent, command, parentDirLength);
    parent[parentDirLength] = '\0';
    int ret = chdir(parent);
    chdir(current);
    if (ret != -1)
        return 1;
    return 0;
}

int discover(char **command, char *home, int endindex)
{
    int i = 0, j = 0, count = 0;
    int d_flag = 0, f_flag = 0, dot_flag = 0, dot_dot_flag = 0, tilde_flag = 0;

    for (i = 0; i < endindex; i++)
    {
        if (strcmp(command[i], "-d") == 0)
            d_flag = 1;
        else if (strcmp(command[i], "-f") == 0)
            f_flag = 1;
        else if (strcmp(command[i], ".") == 0)
            dot_flag = 1;
        else if (strcmp(command[i], "..") == 0)
            dot_dot_flag = 1;
        else if (strcmp(command[i], "~") == 0)
            tilde_flag = 1;
    }

    if (!d_flag && !f_flag) // in case not explicitly mentioned about directory and file flags, consider default case as both
    {
        d_flag = 1;
        f_flag = 1;
    }

    char parent[1000], current[1000];
    getcwd(current, 1000);
    chdir("..");
    getcwd(parent, 1000);
    chdir(current);

    if (dot_flag)
        count++;
    if (dot_dot_flag)
        count++;
    if (tilde_flag)
        count++;

    for (i = 1; i < endindex; i++)
    {
        if (strcmp(command[i], "-d") == 0 || strcmp(command[i], "-f") == 0 || strcmp(command[i], ".") == 0 || strcmp(command[i], "..") == 0 || strcmp(command[i], "~") == 0)
            continue;

        int ret = chdir(command[i]); // find out whether user has given a file as an input or a directory as an input.
        if (ret == -1)               // either a directory with that name does not exist or it is a file.
        {
            struct stat info;
            stat(command[i], &info);
            if (S_ISREG(info.st_mode))       // file confirmed. Search Flag set to True for Searching.
            {
                count++;
                if (check_absolute_path(command[i]))        // check whether absolute path of file is given 
                    printf("%s\n", command[i]);
                else
                    find(command[i], d_flag, f_flag, 1);
            }
            else
                printf("\033[0;31mdiscover: %s: No such file or directory\033[0;37m\n", command[i]);
            continue;
        }
        chdir(current);
        count++;
        find(command[i], d_flag, f_flag, 0);
    }

    if (count == 0) // default case when no arguments are given handled
    {
        printf(".\n");
        find(current, d_flag, f_flag, 0);
        return endindex;
    }

    if (dot_flag)
    {
        printf(".\n");
        find(current, d_flag, f_flag, 0);
    }
    if (dot_dot_flag)
    {
        printf("..\n");
        find(parent, d_flag, f_flag, 0);
    }
    if (tilde_flag)
    {
        printf("%s\n", home);
        find(home, d_flag, f_flag, 0);
    }
    return endindex;
}