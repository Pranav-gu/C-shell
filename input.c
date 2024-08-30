#include "headers.h"

void die(const char *s)
{
    perror(s);
    exit(1);
}

struct termios orig_termios;
void disableRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

void enableRawMode()
{
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        die("tcgetattr");
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON);
    raw.c_lflag &= ~ECHO;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}

void input(char *command, char *hostname, char *username, char *home)
{
    char *inp = malloc(sizeof(char) * 100);
    char c, store[1000];
    setbuf(stdout, NULL);
    enableRawMode();
    memset(inp, '\0', 100);
    memset(store, '\0', 1000);
    int pt = 0;
    while (read(STDIN_FILENO, &c, 1) == 1)
    {
        if (iscntrl(c))
        {
            if (c == 10)                    // Newline
            {
                inp[pt++] = '\n';
                printf("\n");
                break;
            }
            else if (c == 27)
            {
                char buf[3];
                buf[2] = 0;
                if (read(STDIN_FILENO, buf, 2) == 2)             // length of escape code (arrow keys)
                {
                    printf("arrow key: %s", buf);
                }
            }
            else if (c == 127)              // Backspace
            {
                if (pt > 0)
                {
                    printf("\b");
                    printf(" ");
                    printf("\b");
                    inp[--pt] = '\0';
                }
            }
            else if (c == 9)                // Tab Character
            {
                autocompletion(inp, store);
                char temp[1000];
                strcpy(temp, store);
                char *str = strtok(temp, "\n");
                char *str1 = strtok(NULL, "\n");
                if (str1 == NULL)
                {
                    for (int i = 0; i < strlen(store); i++)
                        inp[pt++] = store[i];
                    printf("%s ", store);
                }
                else
                {
                    printf("\n%s", store);
                    disableRawMode();
                    prompt(hostname, username, home);
                    enableRawMode();
                    printf("%s", inp);
                }
            }
            else if (c == 4)                // Ctrl D
            {
                printf("\n");
                disableRawMode();
                exit(0);
            }
            else if (c == 43)                // Ctrl C
            {
                printf("%c\n", c);
                disableRawMode();
                // exit(0);
                return;
            }
        }
        else
        {
            inp[pt++] = c;
        }

        if (c != 9)
            printf("%c", c);
    }
    disableRawMode();

    strcpy(command, inp);
    free(inp);
    return;
}