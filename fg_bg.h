#ifndef _FG_BG_
#define _FG_BG_

void fg(char **args, int startindex, int endindex, int *fg_time);
// void fg(char *command, int startindex, int endindex);
// void bg(char **args, int i);
void bg(char **args, int i, char *hostname, char *username, char *home, char *cdstore, int *fg_time);

#endif