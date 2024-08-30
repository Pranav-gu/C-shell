#include "headers.h"

void implement_ls(char *directory, int a_flag, int l_flag)
{
    DIR *d;
    struct dirent *x;
    d = opendir(directory);
    if (d == NULL)
    {
        printf("\033[0;31mls: cannot access %s: No such file or directory\033[0;37m\n", directory);
        return;
    }
    while ((x = readdir(d)) != NULL)
    {
        if (!a_flag && (x->d_name[0] == '.'))
            continue;
        if (l_flag)
        {
            struct stat info;

            char path[1000];
            strcpy(path, directory);
            strcat(path, "/");
            strcat(path, x->d_name);

            stat(path, &info);
            if (S_ISREG(info.st_mode))
                printf("-");
            else if (S_ISDIR(info.st_mode))
                printf("d");
            else if (S_ISLNK(info.st_mode))
                printf("l");
            else if (S_ISBLK(info.st_mode))
                printf("b");
            else if (S_ISCHR(info.st_mode))
                printf("c");

            if (info.st_mode && S_IRUSR)
                printf("r");
            else
                printf("-");
            if (info.st_mode && S_IWUSR)
                printf("w");
            else
                printf("-");
            if (info.st_mode && S_IXUSR)
                printf("x");
            else
                printf("-");
            if (info.st_mode && S_IRGRP)
                printf("r");
            else
                printf("-");
            if (info.st_mode && S_IWGRP)
                printf("w");
            else
                printf("-");
            if (info.st_mode && S_IXGRP)
                printf("x");
            else
                printf("-");
            if (info.st_mode && S_IROTH)
                printf("r");
            else
                printf("-");
            if (info.st_mode && S_IWOTH)
                printf("w");
            else
                printf("-");
            if (info.st_mode && S_IXOTH)
                printf("x ");
            else
                printf("- ");

            printf("%ld\t", (long int)info.st_nlink);

            if (info.st_uid == 0)
                printf("root root ");
            else
            {
                struct passwd *pw = getpwuid(info.st_uid);
                struct group *gr = getgrgid(info.st_gid);
                printf("%s ", pw->pw_name);
                printf("%s ", gr->gr_name);
            }
            printf("%10ld ", info.st_size);
            char *str = ctime(&info.st_mtime), *str1;
            str1 = strtok(str, " ");
            while (str1 != NULL)
            {
                str1 = strtok(NULL, " ");
                if (str1 == NULL || (str1[0] == '2' && str1[1] == '0' && str1[2] == '2'))
                    break;
                printf("%s ", str1);
            }

            if (S_ISREG(info.st_mode) || S_ISLNK(info.st_mode))
                printf("\033[0;32m%s\033[0;37m\n", x->d_name);
            else if (S_ISDIR(info.st_mode) || S_ISBLK(info.st_mode) || S_ISCHR(info.st_mode))
                printf("\033[0;34m%s\033[0;37m\n", x->d_name);
        }
        else
        {
            struct stat info;
            char path[1000];
            strcpy(path, directory);
            strcat(path, "/");
            strcat(path, x->d_name);
            stat(path, &info);
            if (S_ISREG(info.st_mode) || S_ISLNK(info.st_mode))
                printf("\033[0;32m%s\033[0;37m\n", x->d_name);
            else if (S_ISDIR(info.st_mode) || S_ISBLK(info.st_mode) || S_ISCHR(info.st_mode))
                printf("\033[0;34m%s\033[0;37m\n", x->d_name);
        }
    }
    closedir(d);
}

int ls(char **command, char *home, int endindex)
{
    int dot_flag = 0, dot_dot_flag = 0, tilde_flag = 0, a_flag = 0, l_flag = 0;
    for (int i = 1; i < endindex; i++)
    {
        if (strcmp(command[i], ".") == 0)
            dot_flag = 1;
        else if (strcmp(command[i], "..") == 0)
            dot_dot_flag = 1;
        else if (strcmp(command[i], "~") == 0)
            tilde_flag = 1;
        else if (strcmp(command[i], "-a") == 0)
            a_flag = 1;
        else if (strcmp(command[i], "-l") == 0)
            l_flag = 1;
        else if (strcmp(command[i], "-al") == 0 || strcmp(command[i], "-la") == 0)
        {
            a_flag = 1;
            l_flag = 1;
        }
    }

    char current[1000], parent[1000];
    getcwd(current, 1000);
    chdir("..");
    getcwd(parent, 1000);
    chdir(current);

    int count = 0;
    if (dot_flag)
        count++;
    if (dot_dot_flag)
        count++;
    if (tilde_flag)
        count++;

    for (int i = 1; i < endindex; i++)
    {
        if (strcmp(command[i], ".") == 0 || strcmp(command[i], "..") == 0 || strcmp(command[i], "~") == 0 || strcmp(command[i], "-a") == 0 || strcmp(command[i], "-l") == 0 || strcmp(command[i], "-al") == 0 || strcmp(command[i], "-la") == 0)
            continue;

        int ret = chdir(command[i]); // find out whether user has given a file as an input or a directory as an input.
        if (ret == -1)               // either a directory with that name does not exist or it is a file.
        {
            struct stat info;
            stat(command[i], &info);
            if (S_ISREG(info.st_mode)) // file confirmed
                printf("%s\n", command[i]);
            else
                printf("\033[0;31mls: cannot access %s: No such file or directory\033[0;37m\n", command[i]);
            continue;
        }
        chdir(current);
        count++;
        if (count > 1)
            printf("%s: \n", command[i]);
        implement_ls(command[i], a_flag, l_flag);
    }

    if (count == 0) // case when .. , . , ~ are not explicitly called in ls. Work for Current Directory
        implement_ls(current, a_flag, l_flag);

    if (dot_flag)
    {
        if (count > 1)
            printf(".: \n");
        implement_ls(current, a_flag, l_flag);
    }
    if (dot_dot_flag)
    {
        if (count > 1)
            printf("..: \n");
        implement_ls(parent, a_flag, l_flag);
    }
    if (tilde_flag)
    {
        if (count > 1)
            printf("~: \n");
        implement_ls(home, a_flag, l_flag);
    }

    return endindex;
}