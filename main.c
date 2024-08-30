#include "headers.h"

ptr *job_linked_list;
job_track foreground_proc;
int foreground;
int shell_pid;

void prompt(char *hostname, char *username, char *home)
{
    char current[1000];
    getcwd(current, 1000);
    int curr_len = strlen(current), home_len = strlen(home);
    if (strcmp(home, current) == 0)
    {
        printf("\033[1;32m%s@%s:~$\033[0;37m", username, hostname);
        return;
    }

    if (home_len >= curr_len)
        printf("\033[1;32m%s@%s:%s$\033[0;37m", username, hostname, current);
    else
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
        if (!flag)
            printf("\033[1;32m%s@%s:%s$\033[0;37m", username, hostname, current);
        else
        {
            char temp[1000];
            int j = 0;
            for (j = i; j < curr_len; j++)
                temp[j-i] = current[j];
            temp[j-i] = '\0';
            printf("\033[1;32m%s@%s:~%s$\033[0;37m", username, hostname, temp);
        }
    }
    return;
}

int main()
{
    char hostname[1000], username[1000], home[1000], cdstore[1000];
    int fg_time = 0;
    gethostname(hostname, 1000);
    getlogin_r(username, 1000);
    getcwd(home, 1000);
    getcwd(cdstore, 1000);
    job_linked_list = (ptr *)malloc(sizeof(ptr));
    job_linked_list->size = 0;
    job_linked_list->front = NULL;

    shell_pid = getpid();
    while(1)
    {
        prompt(hostname, username, home);
        if (fg_time != 0)
        {
            printf("took %ds> ", fg_time);
            fg_time = 0;
        }

        (void) signal(SIGINT, signal_handle);
        (void) signal(SIGTSTP, signal_handle);
        (void) signal(SIGTTIN, signal_handle);
        (void) signal(SIGHUP, signal_handle);

        foreground = 0;
        char command[1000];
        // if (fgets(command, 1000, stdin) == NULL)
        // {
            // printf("\n");
            // exit(0);
        // }

        input(command, hostname, username, home);

        if (strcmp(command, "\n") == 0)
            continue;
        update_history(command, home);
        master(command, home, cdstore, hostname, username, &fg_time);
    }
    free(job_linked_list);
    return 0;
}