#include "headers.h"

void rel_path(char *current, char *home)
{
    char temp[1000];
    strcpy(temp, current);

    // parent directory extraction
    char parent[1000];
    const char *lastSlash = strrchr(temp, '/');
    if (lastSlash == NULL)
        return;
    size_t parentDirLength = lastSlash - temp;
    strncpy(parent, temp, parentDirLength);
    parent[parentDirLength] = '\0';

    int curr_len = strlen(parent), home_len = strlen(home);

    if (strcmp(home, parent) == 0)
    {
        strcpy(current, "~");
        strcat(current, lastSlash);
        return;
    }

    if (home_len < curr_len)
    {
        int i = 0, flag = 1;
        for (i = 0; i < home_len; i++)
        {
            if (home[i] != current[i])
            {
                flag = 0;
                break;
            }
        }
        if (flag)
        {
            char temp1[1000];
            int j = 0;
            for (j = i; j < curr_len; j++)
                temp1[j - i] = current[i];
            temp1[j - i] = '\0';
            strcpy(current, temp1);
        }
    }
    if (strcmp(current, temp) != 0)
        strcat(current, lastSlash);
    return;
}

void tostring(char str[], int num)
{
    int i, rem, len = 0, n;

    n = num;
    while (n != 0)
    {
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem + '0';
    }
    str[len] = '\0';
}

int pinfo(char **command, char *home, int endindex)
{
    int pid = getpid(), i = 0, j = 0;
    if (endindex > 2)
    {
        printf("\033[0;31mpinfo: Too many arguments\033[0;37m\n");
        return endindex;
    }

    if (endindex == 2)
    {
        pid = atoi(command[1]);
        if (pid == 0)
        {
            printf("\033[0;31mpinfo: %s is not a Number\033[0;37m\n", command[1]);
            return endindex;
        }
    }
    char path[1000], current[1000], str[1000], *tokens[100], *str1, exec_path[1000], str_pid[1000], link_path[1000];
    tostring(str_pid, pid);

    strcpy(path, "/proc/");
    strcat(path, str_pid);
    getcwd(current, 1000);
    if (chdir(path) == -1)
    {
        printf("\033[0;31mpinfo: process with pid %d does not exist\033[0;37m\n", pid);
        return endindex;
    }
    FILE *fp;
    fp = fopen("./stat", "r");
    while (!feof(fp))
    {
        char ch = fgetc(fp);
        str[i++] = ch;
    }
    str[i] = '\0';

    snprintf(link_path, sizeof(link_path), "/proc/%d/exe", pid);
    ssize_t path_length = readlink(link_path, exec_path, 999);
    if (path_length == -1)
    {
        perror("\033[0;31mFailed to read symbolic link\033[0;37m");
        chdir(current);
        return endindex;
    }
    exec_path[path_length] = '\0';

    rel_path(exec_path, home);
    str1 = strtok(str, " ");
    while (str1 != NULL)
    {
        tokens[j++] = str1;
        str1 = strtok(NULL, " ");
    }
    chdir(current);
    printf("pid: %d\n", pid);
    printf("process status : %s", tokens[2]);

    int fd = open("/dev/tty", O_RDONLY);
    if (getpgrp() == tcgetpgrp(fd))
        printf("+");
    printf("\nmemory : %s\n", tokens[22]);
    printf("executable path : %s\n", exec_path);
    return endindex;
}