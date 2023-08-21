#include "headers.h"

#define ERROR_HISTORY_NOT_OPENED_RET_PAIR      \
    if (!histFile)                             \
    {                                          \
        printf("Could not open pastevents\n"); \
        return NULL;                           \
    }
#define ERROR_HISTORY_NOT_OPENED_RET_VOID      \
    if (!histFile)                             \
    {                                          \
        printf("Could not open pastevents\n"); \
        return;                                \
    }

char *replace_pastevents(char *arg);

char **allocate_memory()
{
    char **history = (char **)malloc(MAX_HISTORY * sizeof(char *));
    for (int i = 0; i < MAX_HISTORY; i++)
        history[i] = (char *)malloc(ARG_MAX * sizeof(char));
    return history;
}

void deallocate_memory(char **history)
{
    for (int i = 0; i < MAX_HISTORY; i++)
        free(history[i]);
    free(history);
}

char *get_history_path()
{
    char *HIST_PATH = (char *)malloc(PATH_MAX * sizeof(char));
    strcpy(HIST_PATH, HOME_DIR);
    strcat(HIST_PATH, "/pastevents");
    return HIST_PATH;
}

historyPair *read_history()
{
    char *HIST_PATH = get_history_path();

    FILE *histFile = fopen(HIST_PATH, "a+");
    ERROR_HISTORY_NOT_OPENED_RET_PAIR
    free(HIST_PATH);

    int histCount = 0;
    char buffer[ARG_MAX];
    int cmdLen;

    char **history = allocate_memory();
    historyPair *pair = (historyPair *)malloc(1 * sizeof(historyPair));

    while (fgets(buffer, ARG_MAX, histFile) && histCount < MAX_HISTORY)
        strcpy(history[histCount++], buffer);

    pair->histCount = histCount;
    pair->history = history;

    return pair;
}

void save_command(char *cmd)
{
    char *command = strdup(cmd);
    command = replace_pastevents(command);
    if (!strcmp(command, ""))
        return;

    command = rstrip(command, ';');

    char *HIST_PATH = get_history_path();

    historyPair *pair = read_history();
    char **history = pair->history;
    int histCount = pair->histCount;

    char *prev = history[max(histCount - 1, 0)];
    prev = rstrip(prev, '\n');
    command = rstrip(command, '\n');

    if (!strcmp(prev, command))
        return;

    prev[strlen(prev)] = '\n';

    if (histCount == MAX_HISTORY)
    {
        FILE *histFile = fopen(HIST_PATH, "w");
        ERROR_HISTORY_NOT_OPENED_RET_VOID
        for (int i = 1; i < MAX_HISTORY; i++)
            fputs(history[i], histFile);
        fclose(histFile);
    }

    FILE *histFile = fopen(HIST_PATH, "a");
    ERROR_HISTORY_NOT_OPENED_RET_VOID
    fputs(command, histFile);
    putc('\n', histFile);
    fclose(histFile);

    deallocate_memory(history);
    free(pair);
    free(HIST_PATH);
    free(command);
}

char *replace_pastevents_execute(char *arg)
{
    historyPair *pair = read_history();
    char *executeCmd = "pastevents execute ";

    int i = 0, j = 0;
    for (; i < strlen(arg); i++)
    {
        if (arg[i] == executeCmd[j])
        {
            while (j < strlen(executeCmd) && arg[i] == executeCmd[j])
            {
                i++;
                j++;
            }
            if (executeCmd[j] == '\0')
                break;
        }
        j = 0;
    }

    int cmdNum = -1;
    sscanf(arg + i, "%d", &cmdNum);

    if (j != 0)
    {
        if (cmdNum == -1)
        {
            deallocate_memory(pair->history);
            free(pair);
            free(arg);
            return strdup("");
        }
        char *newCmd = (char *)calloc(ARG_MAX, sizeof(char));
        strcpy(newCmd, arg);
        newCmd[i - 20 + 1] = '\0';

        if (cmdNum <= pair->histCount)
        {
            strcat(newCmd, rstrip(pair->history[pair->histCount - cmdNum], '\n'));
            strcat(newCmd, ";");
        }

        while (arg[i] && arg[i] != ';')
            i++;
        strcat(newCmd, arg + i + 1);
        free(arg);
        arg = newCmd;
    }

    deallocate_memory(pair->history);
    free(pair);

    return strip(strip(arg, '\n'), ' ');
}

char *replace_pastevents(char *arg)
{
    char *executeCmd = "pastevents";
    const int argLen = strlen(arg);

    int i = 0, j = 0;
    for (; i < argLen; i++)
    {
        if (arg[i] == executeCmd[j])
        {
            while (j < strlen(executeCmd) && arg[i] == executeCmd[j])
            {
                i++;
                j++;
            }
            if (executeCmd[j] == '\0')
                break;
        }
        j = 0;
    }

    if (j != 0)
    {
        char *temp = strdup(arg + i);
        arg[i - 11 + 1] = '\0';
        i = 0;
        while (temp[i] != ';' && i < strlen(temp))
            i++;

        strcat(arg, temp + i);
        free(temp);
    }
    return strip(strip(arg, '\n'), ' ');
}

void delete_history()
{
    char *HIST_PATH = get_history_path();
    FILE *histFile = fopen(HIST_PATH, "w");
    ERROR_HISTORY_NOT_OPENED_RET_VOID
    free(HIST_PATH);
    fclose(histFile);
}

void print_history()
{
    historyPair *pair = read_history();
    for (int i = 0; i < pair->histCount; i++)
        printf("%s", pair->history[i]);

    deallocate_memory(pair->history);
    free(pair);
    return;
}