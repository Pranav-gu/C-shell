#include "headers.h"

int sig(char **command, int endindex)
{
    int flag = 0;
    pid_t pid;
    job_track *list = job_linked_list->front;
    while(list != NULL)
    {
        if (list->job_no == atoi(command[1]))
        {
            flag = 1;
            pid = list->pid;
            break;
        }
        list = list->next;
    }
    if (flag)
    {
        if (kill(pid, atoi(command[2])) < 0)
            printf("\033[0;31mKill Error\033[0;37m\n");
    }
    else
        printf("\033[0;31mProcess with Job No %d does not exist\033[0;37m\n", atoi(command[1]));
    return endindex;
}