#include "headers.h"

char *rstrip(char *string, char letter)
{
    char *str = strdup(string);
    int len = strlen(str);
    while (str[len - 1] == letter)
        str[--len] = '\0';
    return str;
}

char *lstrip(char *string, char letter)
{
    char *str = strdup(string);
    int i = 0;
    while (str[i++] == letter)
        ;
    return str + i - 1;
}

char *strip(char *str, char letter)
{
    return lstrip(rstrip(str, letter), letter);
}

void print_error(char *msg)
{
    printf(ANSI_FG_COLOR_RED "%s" ANSI_COLOR_RESET "\n", msg);
}

/*Get the absolute path of the given argument. Can return null path. Free the argument when possible*/
char *get_abs_path(char *path)
{
    char *absPath = (char *)calloc(PATH_MAX, sizeof(char));

    if ((path[0] == '/'))
    {
        strcpy(absPath, path);
        return absPath;
    }

    if (!strcmp(path, "-"))
    {
        !strcmp(PREV_DIR, "") && strcpy(absPath, PREV_DIR);
        return absPath;
    }

    if (path[0] == '~')
    {
        strcpy(absPath, HOME_DIR);
        strcat(absPath, "/");
        path += 1;
    }

    strcat(absPath, path);
    return absPath;
}

char *get_rel_path(char *path)
{
    char *dup = strdup(path);
    if (!strcmp(path, HOME_DIR))
    {
        dup[0] = '~';
        dup[1] = '\0';
        return dup;
    }
    if (!strncmp(HOME_DIR, path, strlen(HOME_DIR)))
    {
        dup = dup + strlen(HOME_DIR) - 1;
        dup[0] = '~';
        dup[1] = '/';
    }
    return dup;
}

char *remove_whitespaces(char *text)
{
    char *str = strdup(text);
    int i = 0, j = 0;
    int n = strlen(str);
    while (j < n)
    {
        while (str[j] == ' ' && str[j + 1] == ' ')
            j++;
        if (i != j)
            str[i] = str[j];
        i++;
        j++;
    }
    str[i] = '\0';
    free(text);
    return strip(str, ' ');
}

void print_aligned(long num, int digits)
{
    int copy = num;
    int dg = 1;
    for (int copy = num; copy /= 10; dg++)
        ;

    for (int i = 0; i < digits - dg; i++)
        printf(" ");
    printf("%ld ", num);
}

char *trim(char *string, char *prefix)
{
    char *new = strdup(string);
    int i = 0;
    while (new[i] == prefix[i])
        i++;
    new = new + i + (i == 0 ? 0 : 1);
    return new;
}

void print_last_exec_output()
{
    long bytes = 0;
    // FILE *output = fopen("/home/amey/output", "a+");
    FILE *output = fopen(PREV_COMMAND_OUTPUT, "a+");
    if (output < 0)
    {
        print_error("Not able to open previous output buffer");
        return;
    }

    if (output)
    {
        char buffer[1024];
        while (fgets(buffer, 1024, output))
        {
            printf("%s", buffer);
            bytes += strlen(buffer);
        }

        if (bytes)
            printf("\n");
        fclose(output);
    }
    output = fopen(PREV_COMMAND_OUTPUT, "w");
    if (output < 0)
    {
        print_error("Not able to open previous output buffer");
        return;
    }
    fputs("", output);
    fclose(output);

    return;
}

int readString(const char *str, char *res)
{
    int i = 0, j = 0;
    while (str[i] != '\'' && str[i] != '\"')
        i++;
    i++;

    while (str[i] != '\'' && str[i] != '\"')
        res[j++] = str[i++];
    res[j] = 0;
    i++;
    return i;
}



void insert_process(int pid, int argc, char *argv, char isBg)
{
    processNode *newNode = (processNode *)calloc(1, sizeof(processNode));
    newNode->pid = pid;
    newNode->isBg = isBg;
    newNode->spawnTime = time(NULL);
    strcpy(newNode->pName, argv);

    if (Processes->head == NULL)
    {
        Processes->head = newNode;
        Processes->tail = newNode;
        Processes->size += 1;
        return;
    }

    processNode *tail = Processes->tail;
    tail->next = newNode;
    newNode->prev = tail;

    Processes->tail = newNode;

    Processes->size += 1;
    return;
}

processNode *get_process_with_id(int pid)
{
    processNode *temp = Processes->head;
    while (temp)
    {
        if (temp->pid == pid)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

void remove_process_with_id(int pid)
{
    processNode *temp = Processes->head;
    processNode *prev = NULL;
    while (temp)
    {
        if (temp->pid == pid)
        {
            if (prev == NULL)
            {
                Processes->head = temp->next;
                free(temp);
                Processes->size -= 1;
                return;
            }
            prev->next = temp->next;
            free(temp);
            Processes->size -= 1;
            return;
        }
        prev = temp;
        temp = temp->next;
    }
}
