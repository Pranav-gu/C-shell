#include "headers.h"

int handle_bg(char **command, int endindex)
{
    int flag = 0;
    pid_t pid;
    if (endindex > 2)
    {
        printf("\033[0;31mToo many Arguments\n");
        return endindex;
    }
    if (endindex == 1)
    {
        printf("\033[0;31mPlease Mention Job Number of Process\033[0;37m\n");
        return endindex;
    }
    int job = atoi(command[1]);
    if (job < 0)
    {
        printf("\033[0;31mEnter a Number\033[0;37m\n");
        return endindex;
    }
    
    job_track *list = job_linked_list->front;

    while(list != NULL)
    {
        if (list->job_no == job)
        {
            flag = 1;
            pid = list->pid;
            break;
        }
        list = list->next;
    }
    if (!flag)
        printf("\033[0;31mProcess with job no %d Does not Exist\033[0;37m\n", job);
    else
    {
        if (kill(pid, SIGCONT) < 0)
            printf("\033[0;31mKill Error\033[0;37m\n");
        
    }
    return endindex;
}