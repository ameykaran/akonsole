#include "headers.h"

#define ERROR_HISTORY_NOT_OPENED               \
    if (!histFile)                             \
    {                                          \
        printf("Could not open pastevents\n"); \
        exit(1);                               \
    }

extern char HOME_DIR[PATH_MAX];

char **allocate_memory()
{
    char **history = (char **)malloc(MAX_HISTORY * sizeof(char *));
    for (int i = 0; i < MAX_HISTORY; i++)
        history[i] = (char *)malloc(ARG_MAX * sizeof(char));
    return history;
}

char *gen_history_path()
{
    char *HIST_PATH = (char *)malloc(PATH_MAX * sizeof(char));
    strcpy(HIST_PATH, HOME_DIR);
    strcat(HIST_PATH, "/pastevents");
    return HIST_PATH;
}

struct pair *read_history()
{
    char *HIST_PATH = gen_history_path();

    FILE *histFile = fopen(HIST_PATH, "a+");
    ERROR_HISTORY_NOT_OPENED

    int histCount = 0;
    char buffer[ARG_MAX];
    int cmdLen;

    char **history = allocate_memory();
    struct pair *p = (struct pair *)malloc(1 * sizeof(struct pair));

    while (fgets(buffer, ARG_MAX, histFile) && histCount < MAX_HISTORY)
        strcpy(history[histCount++], buffer);

    p->histCount = histCount;
    p->history = history;
}

void save_command(char *command)
{
    // TODO check if pastevents command
    if (!strcmp(command, "pastevents"))
        return;

    char *HIST_PATH = gen_history_path();

    struct pair *p = read_history();
    char **history = p->history;
    int histCount = p->histCount;

    char *prev = history[max(histCount - 1, 0)];
    rstrip(prev, '\n');

    if (!strcmp(prev, command))
        return;

    prev[strlen(prev)] = '\n';

    if (histCount == MAX_HISTORY)
    {
        FILE *histFile = fopen(HIST_PATH, "w");
        ERROR_HISTORY_NOT_OPENED
        for (int i = 1; i < MAX_HISTORY; i++)
            fputs(history[i], histFile);
        fclose(histFile);
    }

    FILE *histFile = fopen(HIST_PATH, "a");
    ERROR_HISTORY_NOT_OPENED
    fputs(command, histFile);
    putc('\n', histFile);
    fclose(histFile);
}