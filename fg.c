#include "headers.h"

int handle_fg(char **command, int endindex)
{
    if (endindex > 2)
    {
        printf("\033[0;31mToo many Arguments\033[0;37m\n");
        return endindex;
    }
    if (endindex == 1)
    {
        printf("\033[0;31mToo few Arguments\033[0;37m\n");
        return endindex;
    }
    int job_no = atoi(command[1]);
    if (job_no < 0)
    {
        printf("\033[0;31mEnter a Number\033[0;37m\n");
        return endindex;
    }

    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);

    job_track *list = job_linked_list->front;
    pid_t pid, status;
    int i = 0, j = 0, flag = 0;

    while(list != NULL)
    {
        if (list->job_no == job_no)
        {
            flag = 1;
            break;
        }
        list = list->next;
    }
    
    if (!flag)
    {
        printf("\033[0;31mProcess with Job no %d does not exist\033[0;37m\n", job_no);
        return endindex;
    }

    pid = list->pid;
    if (kill(pid, SIGCONT) < 0)                     // process now in running state in background
    {
        printf("\033[0;31mKill Error\033[0;37m\n");
        return endindex;
    }

    foreground = 1;
    tcsetpgrp(STDIN_FILENO, pid);                   // transfer it to foreground
    waitpid(pid, &status, WUNTRACED);
    tcsetpgrp(STDIN_FILENO, shell_pid);


    remove_proc(pid);

    tcsetpgrp(STDIN_FILENO, getpgid(getpid()));
    signal(SIGTTIN, SIG_DFL);
    signal(SIGTTOU, SIG_DFL);
    return endindex;
}