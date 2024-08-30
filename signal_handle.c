#include "headers.h"

void signal_handle(int signal_no)
{
    int curr_pid = getpid();
    if (curr_pid < 0)
    {
        printf("\033[0;31mError\033[0;37m\n");
        return;
    }
    else if (curr_pid != shell_pid)
        return;

    if (signal_no == SIGTSTP) // Ctrl Z
    {
        if (foreground)
        {
            int x, y;
            if ((x = kill(foreground_proc.pid, SIGTTIN)) < 0)
            {
                printf("\033[0;31mKill Error\033[0;37m\n");
                return;
            }
            tcsetpgrp(STDIN_FILENO, getpgid(shell_pid));
            tcsetpgrp(STDOUT_FILENO, getpgid(shell_pid));
            if ((y = kill(foreground_proc.pid, SIGTSTP)) < 0)
            {
                printf("\033[0;31mKill Error\033[0;37m\n");
                return;
            }
            foreground = 0;
        }
    }
    else if (signal_no == SIGINT)
    {
        printf("%c", 3);        // ctrl c print
        if (foreground)
        {
            if (kill(foreground_proc.pid, SIGINT))
            {
                printf("\033[0;31mKill Error\033[0;37m\n");
                return;
            }
            foreground = 0;
        }
    }
    else if (signal_no == SIGTTIN)
        signal(SIGTTIN, SIG_IGN);
    else if (signal_no == SIGHUP)
        printf("SIGHUP Signal Received\n");
    return;
}