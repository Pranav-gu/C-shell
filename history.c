#include "headers.h"

int history(char **command, char *home, int endindex)
{
    int i = 0, j = 0;
    if (endindex > 2)
    {
        printf("\033[0;31mhistory: too many arguments\033[0;37m\n");
        return endindex;
    }

    int val;
    if (endindex == 2)
    {
        val = atoi(command[1]);
        if (val == 0)
        {
            printf("\033[0;31mhistory: %s: numeric argument required\033[0;37m\n", command[1]);
            return endindex;
        }
    }

    FILE *fp;
    char path[1000];
    strcpy(path, home);
    strcat(path, "/history.txt");
    fp = fopen(path, "r");
    if (fp == NULL)
        return endindex;

    char A[1000], ch, *args[100], *str;
    while ((ch = fgetc(fp)) != EOF)
        A[j++] = ch;
    A[j] = '\0';

    str = strtok(A, "\n");
    while (str != NULL)
    {
        args[i++] = str;
        str = strtok(NULL, "\n");
    }
    fclose(fp);

    if (endindex == 2 && val < 20)
    {
        for (j = i - val; j < i; j++)
            printf("%s\n", args[j]);
    }
    else
    {
        for (j = 0; j < i; j++)
            printf("%s\n", args[j]);
    }
    return endindex;
}

void update_history(char *command, char *home)
{
    FILE *fp;
    char path[1000];
    strcpy(path, home);
    strcat(path, "/history.txt");
    fp = fopen(path, "r");
    if (fp == NULL)
        return;

    int i = 0, j = 0;
    char A[1000], ch, *args[100], *str;

    while ((ch = fgetc(fp)) != EOF)
        A[j++] = ch;
    A[j] = '\0';

    str = strtok(A, "\n");
    while (str != NULL)
    {
        args[i++] = str;
        str = strtok(NULL, "\n");
    }
    fclose(fp);

    fp = fopen(path, "w");

    if (i >= 20)
    {
        for (j = i - 19; j < i; j++)
            fprintf(fp, "%s\n", args[j]);
    }
    else
    {
        for (j = 0; j < i; j++)
            fprintf(fp, "%s\n", args[j]);
    }

    char temp[1000];
    strcpy(temp, command);
    str = strtok(temp, "\n");
    if (strcmp(args[i - 1], str) != 0)
        fprintf(fp, "%s", command);

    fclose(fp);
    return;
}