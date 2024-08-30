#ifndef _HEADERS_H
#define _HEADERS_H

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <signal.h>
#include <pwd.h>
#include <time.h>
#include <grp.h>
#include <termios.h>
#include <ctype.h>
#include "fg_bg.h"
#include "master.h"
#include "cd.h"
#include "pw.h"
#include "echo.h"
#include "history.h"
#include "ls.h"
#include "discover.h"
#include "pinfo.h"
#include "signal_handle.h"
#include "redirection.h"
#include "pipe.h"
#include "jobs.h"
#include "sig.h"
#include "bg.h"
#include "fg.h"
#include "autocompletion.h"

typedef struct back_process {
    char command[1000];
    int pid;
    int job_no;
    char state[5];
    struct back_process *next;
} job_track;

typedef struct pointer {
    int size;
    struct back_process *front;
} ptr;

extern int shell_pid;
extern ptr *job_linked_list;

extern job_track foreground_proc;
extern int foreground;

void prompt(char *hostname, char *username, char *home);

void input(char *command, char *hostname, char *username, char *home);

typedef struct TrieNode{
    struct TrieNode *children[56];
    int isLeaf;
}Trie;

#endif