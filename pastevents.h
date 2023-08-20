#ifndef PASTEVENTS_H_
#define PASTEVENTS_H_

#define MAX_HISTORY 15

typedef struct historyPair
{
    int histCount;
    char **history;
} historyPair;

historyPair *read_history();
void save_command(char *command);
char *replace_pastevents_execute(char *arg);

#endif