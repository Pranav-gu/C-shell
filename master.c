#include "headers.h"

void master(char *command, char *home, char *cdstore, char *hostname, char *username, int *fg_time)
{
    char *args[100], *str, copy[1000], copy1[1000], *tokens[100], *tokens1[100];
    int i = 0, count_amper = 0, count_semico = 0, count_pipe = 0, index_semico[1000], count_redir = 0;

    index_semico[0] = -1; // Initialise with some Random values

    strcpy(copy, command);
    strcpy(copy1, command);

    for (int j = 0; j < strlen(command); j++)
    {
        if (command[j] == '&')
            count_amper++;
        else if (command[j] == '|') // pipe to be handled
            count_pipe++;
        else if (command[j] == ';')
            index_semico[count_semico++] = j;
        else if (command[j] == '>' && command[j+1] == '>')
        {
            count_redir++;
            j++;
        }
        else if (command[j] == '<' || command[j] == '>')
            count_redir++;
    }

    int jump_flag = 0;

    if (count_amper > 0)
    {
        char temp[1000];
        strcpy(temp, copy);
        for (int x = 0; x < count_amper; x++)
        {
            if (x == 0)
                str = strtok(copy, "&\n");
            else 
                str = strtok(NULL, "&\n");
            args[i++] = str;
        }
        str = strtok(NULL, "&\n");
        args[i] = str;
        if (args[i] == NULL)
            bg(args, i, hostname, username, home, cdstore, fg_time);
        else                                        // last foreground process to be handled
        {
            strcpy(copy1, args[i]);
            jump_flag = 1;
            goto bottom;
        bg_execute:
            bg(args, i, hostname, username, home, cdstore, fg_time);
        }
        return;
    }

bottom:
    i = 0;
    str = strtok(copy1, ";\n");
    while (str != NULL)
    {
        tokens[i++] = str;
        str = strtok(NULL, ";\n");
    }

    for (int j = 0; j < i; j++)
    {
        int k = 0, redirect_flag = 0;
        char *tokens_temp = tokens[j];
        char *strtemp = strtok(tokens[j], " \r\t\n\v");
        while (strtemp != NULL)
        {
            tokens1[k++] = strtemp;
            strtemp = strtok(NULL, " \r\t\n\v");
        }
        
        if (count_pipe > 0)
        {
            piping(tokens1, k, home, cdstore, fg_time, count_pipe);
            continue;
        }

        for (int x = 0; x < k; x++)
        {
            if (strcmp(tokens1[x], "<") == 0 || strcmp(tokens1[x], ">") == 0 || strcmp(tokens1[x], ">>") == 0)
            {
                redirection(tokens1, k, home, cdstore, fg_time);
                x = k;
                redirect_flag = 1;
            }
        }

        if (redirect_flag)
            continue;
        
        for (int x = 0; x < k; x++)
        {
            if (strcmp(tokens1[x], "cd") == 0)
                x = cd(tokens1, home, cdstore, k);
            else if (strcmp(tokens1[x], "pwd") == 0)
            {
                x = pw(tokens1, k);
            }
            else if (strcmp(tokens1[x], "echo") == 0)
            {
                x = echo(tokens1, x, k);   
            }
            else if (strcmp(tokens1[x], "ls") == 0)
                x = ls(tokens1, home, k);
            else if (strcmp(tokens1[x], "pinfo") == 0)
                x = pinfo(tokens1, home, k);
            else if (strcmp(tokens1[x], "discover") == 0)
                x = discover(tokens1, home, k);
            else if (strcmp(tokens1[x], "history") == 0)
                x = history(tokens1, home, k);
            else if (strcmp(tokens1[x], "jobs") == 0)
                x = jobs(tokens1, k);
            else if (strcmp(tokens1[x], "sig") == 0)
                x = sig(tokens1, k);
            else if (strcmp(tokens1[x], "bg") == 0)
                x = handle_bg(tokens1, k);
            else if (strcmp(tokens1[x], "fg") == 0)
                x = handle_fg(tokens1, k);
            else
            {
                fg(tokens1, x, k, fg_time);              // string with spaces need to be taken care of
                x = k;
            }
        }
    }
    if (jump_flag)
        goto bg_execute;
    return;
}