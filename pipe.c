#include "headers.h"

void piping(char **command, int end, char *home, char *cdstore, int *fg_time, int count)
{
    int i = 0, j = 0, start_index = 0;
    char *tokens[100], *output[100];
    int fd[2];
    pid_t ret, status;
    int fdd = 0;

    while (count >= 0)
    {
        int redirect_flag = 0;
        if (pipe(fd) < 0)
        {
            printf("\033[0;31mPipe Error\033[0;37m\n");
            return;
        }

        while (i < end && strcmp(command[i], "|") != 0)
            i++;

        for (int x = start_index; x < i; x++) // check if there is any redirection in individual commands
        {
            output[x - start_index] = command[x];
            if (strcmp(command[x], "<") == 0 || strcmp(command[x], ">") == 0 || strcmp(command[x], ">>") == 0)
                redirect_flag = 1;
        }
        output[i - start_index] = NULL;

        ret = fork();

        if (ret == -1)
        {
            printf("\033[0;31mFork Error\033[0;37m\n");
            exit(EXIT_FAILURE);
        }
        else if (ret == 0)
        {
            dup2(fdd, 0);
            if (count > 0)
                dup2(fd[1], 1);
            close(fd[0]);

            if (redirect_flag)
            {
                redirection(output, i - start_index, home, cdstore, fg_time);
                exit(EXIT_SUCCESS);
            }

            if (strcmp(command[start_index], "cd") == 0)
                cd(output, home, cdstore, i - start_index);
            else if (strcmp(command[start_index], "pwd") == 0)
                pw(output, i - start_index);
            else if (strcmp(command[start_index], "echo") == 0)
                echo(output, start_index, i - start_index);
            else if (strcmp(command[start_index], "ls") == 0)
                ls(output, home, i - start_index);
            else if (strcmp(command[start_index], "pinfo") == 0)
                pinfo(output, home, i - start_index);
            else if (strcmp(command[start_index], "discover") == 0)
                discover(output, home, i - start_index);
            else if (strcmp(command[start_index], "history") == 0)
                history(output, home, i - start_index);
            else if (strcmp(command[start_index], "jobs") == 0)
                jobs(output, i - start_index);
            else if (strcmp(command[start_index], "sig") == 0)
                sig(output, i - start_index);
            else if (strcmp(command[start_index], "bg") == 0)
                handle_bg(output, i - start_index);
            else if (strcmp(command[start_index], "fg") == 0)
                handle_fg(output, i - start_index);
            else
                fg(output, 0, i - start_index, fg_time);

            exit(EXIT_SUCCESS);
        }
        else
        {
            waitpid(ret, &status, 0);
            close(fd[1]);
            fdd = fd[0];
            count--;
            i++;
            start_index = i;
        }
    }
    return;
}