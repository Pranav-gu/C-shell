#include "headers.h"

void fg(char **args, int startindex, int endindex, int *fg_time)
{
    char *args1[100];
    int i;
    for (i = 0; i < 100; i++)
        args1[i] = NULL;
    
    for (i = startindex; i < endindex; i++)
        args1[i - startindex] = args[i];

    pid_t ret = fork();
    int status;

    if (ret == 0) // Child Process
    {
        setpgid(0, 0);
        signal(SIGTTIN, SIG_IGN);
        signal(SIGTTOU, SIG_IGN);
        signal(SIGTSTP, signal_handle);
        int res = execvp(args1[0], args1);
        if (res == -1)
            printf("\033[0;31mCommand Not Found\033[0;37m\n");
        exit(1);
    }
    else
    {
        signal(SIGTTIN, SIG_IGN);
        signal(SIGTTOU, SIG_IGN);
        foreground = 1;
        foreground_proc.pid = ret;
        char temp[1000];
        strcpy(temp, args1[0]);
        for (int i = 1; i < endindex - startindex; i++)
        {
            strcat(temp, " ");
            strcat(temp, args1[i]);
        }

        strcpy(foreground_proc.command, temp);
        struct timespec begin, end;
        clock_gettime(0, &begin);

        // tcsetpgrp(STDIN_FILENO, ret);
        tcsetpgrp(STDIN_FILENO, getpgid(ret));
        waitpid(ret, &status, 0);
        // tcsetpgrp(STDIN_FILENO, shell_pid);

        // if (WIFEXITED(status))
        //     printf("Child process exited normally with status %d\n", WEXITSTATUS(status));
        // else
        //     printf("Child process did not exit normally\n");
        if(WIFSTOPPED(status))
        {
            int stop = WSTOPSIG(status);
            if(stop == SIGTSTP)
            {
                job_track *new;
                new = (job_track *)malloc(sizeof(job_track));
                strcpy(new->command, foreground_proc.command);
                new->pid = foreground_proc.pid;
                job_linked_list->size++;
                new->job_no = job_linked_list->size;
                new->next = NULL;
                if (job_linked_list->front == NULL)
                    job_linked_list->front = new;
                else
                {
                    job_track *list = job_linked_list->front;
                    while (list->next != NULL)
                        list = list->next;
                    list->next = new;
                }
            }
        }

        signal(SIGTTIN, SIG_DFL);
        signal(SIGTTOU, SIG_DFL);
        clock_gettime(0, &end);
        *fg_time = end.tv_sec - begin.tv_sec;
        if (!foreground)
            *fg_time = 0;
    }

    return;
}

void bg(char **args, int i, char *hostname, char *username, char *home, char *cdstore, int *fg_time)
{
    char *str;
    char *tokens[100];
    for (int j = 0; j < 100; j++)
        tokens[j] = NULL;

    for (int j = 0; j < i; j++)
    {
        int k = 0, job_index = 0, it = 0;
        char job_store[1000];
        char *temp = args[j];
        str = strtok(args[j], " \t\r\n\v;");
        while (str != NULL)
        {
            tokens[k++] = str;
            str = strtok(NULL, " \t\r\n\v;");
        }

        for (it = 0; it < k; it++)
        {
            int it1;

            for (it1 = 0; it1 < strlen(tokens[it]); it1++)
                job_store[job_index++] = tokens[it][it1];
            job_store[job_index++] = ' ';
        }
        job_store[job_index] = '\0';

        int ret = fork();
        int status;

        if (ret == 0)
        {
            signal(SIGTTOU, SIG_IGN);
            setpgid(0, 0);

            int devNull = open("/dev/null", O_RDWR);
            dup2(devNull, STDIN_FILENO);
            dup2(devNull, STDOUT_FILENO);
            dup2(devNull, STDERR_FILENO);

            close(devNull);

            int res = execvp(tokens[0], tokens);
            if (res == -1)
                printf("\033[0;31mCommand Not Found\033[0;37m\n");
        }
        else
        {
            setpgid(ret, ret);
            job_track *new;
            new = (job_track *)malloc(sizeof(job_track));
            strcpy(new->command, job_store);
            new->pid = ret;

            job_linked_list->size++;
            new->job_no = job_linked_list->size;
            new->next = NULL;
            if (job_linked_list->front == NULL)
                job_linked_list->front = new;
            else
            {
                job_track *list = job_linked_list->front;
                while (list->next != NULL)
                    list = list->next;
                list->next = new;
            }

            // printf("new->pid = %d\n", new->pid);
            // printf("new->job_no = %d\n", new->job_no);
            // printf("new->command = %s\n", new->command); // implement jobs command

            int running = waitpid(ret, &status, WNOHANG);
            while (running == 0)
            {
                running = waitpid(ret, &status, WNOHANG);
                if (running == 0)
                {
                    prompt(hostname, username, home);
                    char command[1000];
                    if (fgets(command, 1000, stdin) == NULL)
                    {
                        printf("\n");
                        exit(0);
                    }
                    if (strcmp(command, "\n") == 0)
                        continue;
                    update_history(command, home);
                    if (strcmp(command, "clear\n") == 0)
                    {
                        system("clear");
                        continue;
                    }
                    master(command, home, cdstore, hostname, username, fg_time);
                }
                else if (running > 0)
                {
                    if (WIFEXITED(status))
                        printf("%s with pid %d exited normally\n", temp, ret);
                    else
                        printf("%s with pid %d exited abnormally\n", temp, ret);
                    remove_proc(ret);
                }
                else
                {
                    if (foreground)
                    {
                        if (WIFEXITED(status))
                            printf("%s\n", job_store);
                        else
                            printf("%s\n", job_store);
                    }
                    if (!foreground)
                        printf("\033[0;31mwaitpid: error\033[0;37m\n");
                }
            }
        }
    }
    return;
}