#include "headers.h"

Trie* make_node()
{
    Trie *new = (Trie *)malloc(sizeof(Trie));
    for (int i = 0; i < 56; i++)
        new->children[i] = NULL;
    new->isLeaf = 0;
    return new;
}

void insert(Trie *root, char *string)
{
    int j = 0, len = strlen(string);
    Trie *T = root;
    for (int i = 0; i < len; i++)
    {
        int x = string[i];
        if (x >= 97 && x <= 122)
            x -= 71;
        else if (x >= 65 && x <= 90)
            x -= 65;
        else if (string[i] == '.')
            x = 52;
        else if (string[i] == '-')
            x = 53;
        else if (string[i] == '_')
            x = 54;
        else if (string[i] == '/')
            x = 55;
        if (T->children[x] == NULL)
            T->children[x] = make_node();
        T = T->children[x];
    }
    T->isLeaf = 1;
    return;
}


void insert_directory(Trie *root)
{
    struct dirent *x;
    DIR *d;
    if ((d = opendir(".")) == NULL)
    {
        printf("\033[0;31mError Reading Directory\033[0;37m\n");
        return;
    }
    while((x = readdir(d)) != NULL)
    {
        char str[100];
        strcpy(str, x->d_name);
        struct stat info;
        stat(x->d_name, &info);
        if (S_ISDIR(info.st_mode))
            strcat(str, "/");
        insert(root, str);
    }
    free(x);
    free(d);
    return;
}

int count_recur(Trie *root, Trie *record)
{
    int z = 0;
    if (root != record && root->isLeaf)
        z += 1;

    for (int i = 0; i < 56; i++)
    {
        if (root->children[i] != NULL)
            z += count_recur(root->children[i], record);
    }
    return z;
}

void autosuggest(struct TrieNode* root, char str[], char store[], char prefix[], int level)
{
    if (root->isLeaf)
    {
        str[level] = '\0';
        if (store[0] == '\0')
            strcpy(store, prefix);
        else
            strcat(store, prefix);
        strcat(store, str);
        strcat(store, "\n");
    }
 
    int i;
    for (i = 0; i < 56; i++)
    {
        if (root->children[i])
        {
            if (i >= 26 && i <= 51)
                str[level] = i + 'a' - 26;
            else if (i >= 0 && i <= 25)
                str[level] = i + 'A';
            else if (i == 52)
                str[level] = '.';
            else if (i == 53)
                str[level] = '-';
            else if (i == 54)
                str[level] = '_';
            else if (i == 55)
                str[level] = '/';
            autosuggest(root->children[i], str, store, prefix, level + 1);
        }
    }
}


int search(Trie *root, char *string, char *store)
{
    int j = 0, len = strlen(string);
    Trie *T = root;
    char str[100], prefix[100];
    memset(prefix, '\0', 100);

    for (int i = 0; i < len; i++)
    {
        int x = string[i];
        prefix[i] = string[i];
        if (x >= 97 && x <= 122)
            x -= 71;
        else if (x >= 65 && x <= 90)
            x -= 65;
        else if (string[i] == '.')
            x = 52;
        else if (string[i] == '-')
            x = 53;
        else if (string[i] == '_')
            x = 54;
        else if (string[i] == '/')
            x = 55;

        if (T->children[x] == NULL)
            return 0;   // 0 means that string would not be autocompleted. Sub String of String itself is not in Directory.
        T = T->children[x];
    }
    if (!T->isLeaf)
    {
        int flag = 1;
        for (int i = 0; i < 56; i++)
        {
            if (T->children[i] != NULL)
            {
                flag = 0;
                break;
            }
        }
        if (!flag)    // autosuggestion or autocompletion.
        {
            int ret = count_recur(T, T);
            if (ret == 1)             // autocomplete to that word
            {
                int index = 0;
                while(1)
                {
                    int flag1 = 0;
                    for (int i = 0; i < 56; i++)
                    {
                        if (T->children[i] != NULL)
                        {
                            flag1 = 1;
                            if (i >= 26 && i <= 51)
                                store[index++] = i + 'a' - 26;
                            else if (i >= 0 && i <= 25)
                                store[index++] = i + 'A';
                            else if (i == 52)
                                store[index++] = '.';
                            else if (i == 53)
                                store[index++] = '-';
                            else if (i == 54)
                                store[index++] = '_';
                            else if (i == 55)
                                store[index++] = '/';
                            T = T->children[i];
                            break;
                        }
                    }
                    if (!flag1)
                        break;
                }
                store[index] = '\0';
                return 2;
            }
            else                      // autosuggestion
            {
                autosuggest(T, str, store, prefix, 0);
                store[strlen(store)] = '\0';
                return 3;
            }
        }
        return 1;    // Neither Autocompletion nor Autosuggestion. String is there in Directory. No Action Required
    }

}

void display(struct TrieNode* root, char str[], int level)
{
    if (root->isLeaf)
    {
        str[level] = '\0';
        printf("\n%s", str);
    }
 
    int i;
    for (i = 0; i < 56; i++)
    {
        if (root->children[i])
        {
            if (i >= 26 && i <= 51)
                str[level] = i + 'a' - 26;
            else if (i >= 0 && i <= 25)
                str[level] = i + 'A';
            else if (i == 52)
                str[level] = '.';
            else if (i == 53)
                str[level] = '-';
            else if (i == 54)
                str[level] = '_';
            else if (i == 55)
                str[level] = '/';
            display(root->children[i], str, level + 1);
        }
    }
}


void deleteTrie(Trie* root)
{   
    for (int i = 0; i < 56; i++)
    {
        if(root->children[i] != NULL)
            deleteTrie(root->children[i]);
    }
    free(root);
    return;
}

void autocompletion(char *command, char *store)
{
    Trie *root = make_node();
    char process[1000], ans[100];
    strcpy(process, command);
    char *str = strtok(process, " \t\r\n\v;|"), *args[100];
    int i = 0;
    while(str != NULL)
    {
        args[i++] = str;
        str = strtok(NULL, " \t\r\n\v;|");
    }
    args[i] = NULL;
    insert_directory(root);
    if (strcmp(command, "") == 0)
    {    
        display(root, ans, 0);
        deleteTrie(root);
        return;
    }

    search(root, args[i-1], store);
    free(str);
    deleteTrie(root);
    return;
}