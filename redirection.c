#include "headers.h"

int min(int x, int y)
{
    return x > y ? y : x;
}

void execute(char **tokens, char *home, char *cdstore, int index, int fd, int *fg_time)
{
    if (strcmp(tokens[0], "cd") == 0)
        cd(tokens, home, cdstore, index);
    else if (strcmp(tokens[0], "pwd") == 0)
        pw(tokens, index);
    else if (strcmp(tokens[0], "echo") == 0)
        echo(tokens, 0, index);
    else if (strcmp(tokens[0], "ls") == 0)
        ls(tokens, home, index);
    else if (strcmp(tokens[0], "pinfo") == 0)
        pinfo(tokens, home, index);
    else if (strcmp(tokens[0], "discover") == 0)
        discover(tokens, home, index);
    else if (strcmp(tokens[0], "history") == 0)
        history(tokens, home, index);
    else if (strcmp(tokens[0], "jobs") == 0)
        jobs(tokens, index);
    else if (strcmp(tokens[0], "sig") == 0)
        sig(tokens, index);
    else if (strcmp(tokens[0], "bg") == 0)
        handle_bg(tokens, index);
    else if (strcmp(tokens[0], "fg") == 0)
        handle_fg(tokens, index);
    else
        fg(tokens, 0, index, fg_time); // string with spaces need to be taken care of
    close(fd);
    exit(EXIT_SUCCESS);
}

void execute1(char **command, char *file_name, int end, int index, char **tokens, char *str)
{
    if (strcmp(command[end - 1], str) == 0)
    {
        printf("\033[0;31msyntax error near unexpected token newline\033[0;37m\n");
        return;
    }
    strcpy(file_name, command[end - 1]);
    for (int i = 0; i < index; i++)
        tokens[i] = command[i];
}

void func1(char **command, int flag, int index, int end, char *home, char *cdstore, int *fg_time)
{
    int fd, count1 = 0, count2 = 0;
    char file_name[1000], *tokens[100];

    if (flag == 0)
    {
        execute1(command, file_name, end, index, tokens, "<");
        pid_t ret, status;
        ret = fork();

        if (ret < 0)
        {
            printf("\033[0;31mFork Error\033[0;37m\n");
            return;
        }
        else if (ret == 0) // child process
        {
            fd = open(file_name, O_RDONLY);
            if (fd == -1)
            {
                printf("\033[0;31mError Opening File\033[0;37m\n");
                exit(EXIT_FAILURE);
            }

            int x = dup2(fd, STDIN_FILENO);
            if (x == -1)
            {
                printf("\033[0;31mDup2 Error\033[0;37m\n");
                exit(EXIT_FAILURE);
            }
            execute(tokens, home, cdstore, index, fd, fg_time);
        }
        else
            waitpid(ret, &status, 0);
    }
    else if (flag == 1)
    {
        execute1(command, file_name, end, index, tokens, ">");
        pid_t ret, status;
        ret = fork();

        if (ret < 0)
        {
            printf("\033[0;31mFork Error\033[0;37m\n");
            return;
        }
        else if (ret == 0) // child process
        {
            fd = open(file_name, O_WRONLY | O_CREAT, 0644);
            if (fd == -1)
            {
                printf("\033[0;31mError Opening File\033[0;37m\n");
                exit(EXIT_FAILURE);
            }

            int x = dup2(fd, STDOUT_FILENO);
            if (x == -1)
            {
                printf("\033[0;31mDup2 Error\033[0;37m\n");
                exit(EXIT_FAILURE);
            }
            execute(tokens, home, cdstore, index, fd, fg_time);
        }
        else
            waitpid(ret, &status, 0);
    }
    else
    {
        execute1(command, file_name, end, index, tokens, ">>");
        pid_t ret, status;
        ret = fork();

        if (ret < 0)
        {
            printf("\033[0;31mFork Error\033[0;37m\n");
            return;
        }
        else if (ret == 0) // child process
        {
            fd = open(file_name, O_RDWR | O_APPEND | O_CREAT, 0644);
            if (fd == -1)
            {
                printf("\033[0;31mError Opening File\033[0;37m\n");
                exit(EXIT_FAILURE);
            }
            lseek(fd, 0, SEEK_END);

            int x = dup2(fd, STDOUT_FILENO);
            if (x == -1)
            {
                printf("\033[0;31mDup2 Error\033[0;37m\n");
                exit(EXIT_FAILURE);
            }
            execute(tokens, home, cdstore, index, fd, fg_time);
        }
        else
            waitpid(ret, &status, 0);
    }
    return;
}

void func2(char **command, int flag, int flag_index, int less_index, int end, char *home, char *cdstore, int *fg_time)
{
    int fd, fd1, count1 = 0, count2 = 0, i = 0, j = 0;
    char file_out[1000], *tokens[100], file_in[1000];

    int index = min(less_index, flag_index);
    for (i = 0; i < index; i++)
        tokens[i] = command[i];

    if (less_index < flag_index) // < appears before > or >>
    {
        if (less_index == flag_index - 1)
        {
            if (flag == 1)
                printf("\033[0;31msyntax error near unexpected token >\033[0;37m\n");
            else
                printf("\033[0;31msyntax error near unexpected token >>\033[0;37m\n");
            return;
        }
        if (strcmp(command[end - 1], ">") == 0)
        {
            if (flag == 1)
                printf("\033[0;31msyntax error near unexpected token newline\033[0;37m\n");
            else
                printf("\033[0;31msyntax error near unexpected token newline\033[0;37m\n");
            return;
        }
        strcpy(file_out, command[flag_index - 1]);
        strcpy(file_in, command[flag_index + 1]);

        pid_t ret, status;
        ret = fork();
        if (ret < 0)
        {
            printf("\033[0;31mFork Error\033[0;37m\n");
            return;
        }
        else if (ret == 0) // child process
        {
            fd = open(file_out, O_RDONLY);
            if (flag == 1)
                fd1 = open(file_in, O_WRONLY | O_CREAT, 0644);
            else
                fd1 = open(file_in, O_WRONLY | O_APPEND | O_CREAT, 0644);

            if (fd == -1 || fd1 == -1)
            {
                printf("\033[0;31mError Opening File\033[0;37m\n");
                return;
            }
            int x = dup2(fd, STDIN_FILENO);
            if (x == -1)
            {
                printf("\033[0;31mDup2 Error\033[0;37m\n");
                return;
            }

            int y = dup2(fd1, STDOUT_FILENO);
            if (y == -1)
            {
                printf("\033[0;31mDup2 Error 2 \033[0;37m\n");
                return;
            }

            if (strcmp(tokens[0], "cd") == 0)
                cd(tokens, home, cdstore, index);
            else if (strcmp(tokens[0], "pwd") == 0)
                pw(tokens, index);
            else if (strcmp(tokens[0], "echo") == 0)
                echo(tokens, 0, index);
            else if (strcmp(tokens[0], "ls") == 0)
                ls(tokens, home, index);
            else if (strcmp(tokens[0], "pinfo") == 0)
                pinfo(tokens, home, index);
            else if (strcmp(tokens[0], "discover") == 0)
                discover(tokens, home, index);
            else if (strcmp(tokens[0], "history") == 0)
                history(tokens, home, index);
            else if (strcmp(tokens[0], "jobs") == 0)
                jobs(tokens, index);
            else if (strcmp(tokens[0], "sig") == 0)
                sig(tokens, index);
            else if (strcmp(tokens[0], "bg") == 0)
                handle_bg(tokens, index);
            else if (strcmp(tokens[0], "fg") == 0)
                handle_fg(tokens, index);
            else
                fg(tokens, 0, index, fg_time); // string with spaces need to be taken care of
            close(fd);
            close(fd1);
            exit(EXIT_SUCCESS);
        }
        else
            waitpid(ret, &status, 0);
    }
    else // < appears after > or >>
    {
        if (less_index == flag_index + 1)
        {
            printf("\033[0;31msyntax error near unexpected token <\033[0;37m\n");
            return;
        }
        if (strcmp(command[end - 1], "<") == 0)
        {
            printf("\033[0;31msyntax error near unexpected token newline\033[0;37m\n");
            return;
        }
        strcpy(file_out, command[end - 1]);
        strcpy(file_in, command[flag_index + 1]);

        pid_t ret, status;
        ret = fork();
        if (ret < 0)
        {
            printf("\033[0;31mFork Error \033[0;37m\n");
            return;
        }
        else if (ret == 0) // child process
        {
            fd = open(file_out, O_RDONLY);
            if (flag == 1)
                fd1 = open(file_in, O_WRONLY | O_CREAT, 0644);
            else
                fd1 = open(file_in, O_WRONLY | O_APPEND | O_CREAT, 0644);

            if (fd == -1 || fd1 == -1)
            {
                printf("\033[0;31mError Opening File \033[0;37m\n");
                return;
            }

            int x = dup2(fd, STDIN_FILENO);
            if (x == -1)
            {
                printf("\033[0;31mDup2 Error \033[0;37m\n");
                return;
            }

            int y = dup2(fd1, STDOUT_FILENO);
            if (y == -1)
            {
                printf("\033[0;31mDup2 Error 2\033[0;37m\n");
                return;
            }

            if (strcmp(tokens[0], "cd") == 0)
                cd(tokens, home, cdstore, index);
            else if (strcmp(tokens[0], "pwd") == 0)
                pw(tokens, index);
            else if (strcmp(tokens[0], "echo") == 0)
                echo(tokens, 0, index);
            else if (strcmp(tokens[0], "ls") == 0)
                ls(tokens, home, index);
            else if (strcmp(tokens[0], "pinfo") == 0)
                pinfo(tokens, home, index);
            else if (strcmp(tokens[0], "discover") == 0)
                discover(tokens, home, index);
            else if (strcmp(tokens[0], "history") == 0)
                history(tokens, home, index);
            else if (strcmp(tokens[0], "jobs") == 0)
                jobs(tokens, index);
            else if (strcmp(tokens[0], "sig") == 0)
                sig(tokens, index);
            else if (strcmp(tokens[0], "bg") == 0)
                handle_bg(tokens, index);
            else if (strcmp(tokens[0], "fg") == 0)
                handle_fg(tokens, index);
            else
                fg(tokens, 0, index, fg_time); // string with spaces need to be taken care of
            close(fd);
            close(fd1);
            exit(EXIT_SUCCESS);
        }
        else
            waitpid(ret, &status, 0);
    }
}

void redirection(char **command, int i, char *home, char *cdstore, int *fg_time)
{
    int less = 0, greater = 0, append = 0, count = 0;
    for (int j = 0; j < i; j++)
    {
        if (strcmp(command[j], "<") == 0)
        {
            count++;
            less = j;
        }
        else if (strcmp(command[j], ">") == 0)
        {
            count++;
            greater = j;
        }
        else if (strcmp(command[j], ">>") == 0)
        {
            count++;
            append = j;
        }
    }

    if (count == 1)
    {
        if (less != 0)
            func1(command, 0, less, i, home, cdstore, fg_time);
        else if (greater != 0)
            func1(command, 1, greater, i, home, cdstore, fg_time);
        else if (append != 0)
            func1(command, 2, append, i, home, cdstore, fg_time);
    }
    else
    {
        if (greater != 0)
            func2(command, 1, greater, less, i, home, cdstore, fg_time);
        else if (append != 0)
            func2(command, 2, append, less, i, home, cdstore, fg_time);
    }
    return;
}