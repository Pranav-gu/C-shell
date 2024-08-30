#include "headers.h"

void remove_proc(int pid)
{
    job_track *list = job_linked_list->front, *list1;
    // int job_no;
    if (list->pid == pid)
    {
        job_linked_list->front = list->next;
        // job_no = 1;
        free(list);
    }
    else
    {
        int flag = 0;                           // removal flag. 0 in the beginning
        list1 = job_linked_list->front;
        list = list->next;
        while(list->next != NULL)
        {
            if (list->pid == pid)
            {
                flag = 1;
                list1->next = list->next;
                // job_no = list->job_no;
                break;
            }
            list = list->next;
            list1 = list1->next;
        }
        if (!flag)
        {
            // job_no = list->job_no;
            list1->next = NULL;
        }
        free(list);
    }
    job_linked_list->size--;
    return;
}

void swap_strings(char *x, char *y)
{
    char temp[1000];
    strcpy(temp, x);
    strcpy(x, y);
    strcpy(y, temp);
    return;
}

void swap_numbers(int *x, int *y)
{
    int temp;
    temp = *x;
    *x = *y;
    *y = temp;
    return;
}

void sort_strings(job_track *A, int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (strcmp(A[i].command, A[j].command) > 0)
            {
                swap_strings(A[i].command, A[j].command);
                swap_strings(A[i].state, A[j].state);
                swap_numbers(&A[i].pid, &A[j].pid);
                swap_numbers(&A[i].job_no, &A[j].job_no);
            }
        }
    }
    for (int j = 0; j < n; j++)
    {
        if (strcmp(A[j].state, "S") == 0)
            printf("[%d] Running %s [%d]\n", A[j].job_no, A[j].command, A[j].pid);
        else if (strcmp(A[j].state, "T") == 0)
            printf("[%d] Stopped %s [%d]\n", A[j].job_no, A[j].command, A[j].pid);
    }
    return;
}

void int_to_string(char str[], int num)
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

void state(char *str, int pid)
{
    char str_pid[1000], path[1000], current[1000], *str1, *tokens[100];
    int i = 0, j = 0;
    int_to_string(str_pid, pid);
    strcpy(path, "/proc/");
    strcat(path, str_pid);
    getcwd(current, 1000);
    if (chdir(path) == -1)
        return;

    FILE *fp;
    fp = fopen("./stat", "r");
    while (!feof(fp))
    {
        char ch = fgetc(fp);
        str[i++] = ch;
    }
    str[i] = '\0';

    str1 = strtok(str, " ");
    while (str1 != NULL)
    {
        tokens[j++] = str1;
        str1 = strtok(NULL, " ");
    }
    chdir(current);
    strcpy(str, tokens[2]);
    return;
}

void implement_jobs(int r_flag, int s_flag)
{
    int i = 0;
    job_track A[1000], *list;
    char str[1000];
    list = job_linked_list->front;
    while (list != NULL)
    {
        state(str, list->pid);
        if ((strcmp(str, "T") == 0 && s_flag) || (strcmp(str, "S") == 0 && r_flag))
        {
            strcpy(A[i].state, str);
            A[i].pid = list->pid;
            A[i].job_no = list->job_no;
            strcpy(A[i].command, list->command);
            i++;
        }
        list = list->next;
        strcmp(str, "\0");
    }

    sort_strings(A, i);
    return;
}


int jobs(char **command, int endindex)
{
    int i = 0, j = 0, r_flag = 0, s_flag = 0;

    if (endindex > 3)
    {
        printf("\033[0;31mToo Many Arguments\033[0;37m\n");
        return endindex;
    }

    for (i = 1; i < endindex; i++)
    {
        if (strcmp(command[i], "-r") == 0)
            r_flag = 1;
        else if (strcmp(command[i], "-s") == 0)
            s_flag = 1;
        else
            printf("\033[0;31mjobs: %s: invalid option\033[0;37m\n", command[i]);
    }

    if (!r_flag && !s_flag) // case when no arguments are given
    {
        r_flag = 1;
        s_flag = 1;
    }
    implement_jobs(r_flag, s_flag);

    return endindex;
}