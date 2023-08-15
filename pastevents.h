#ifndef PASTEVENTS_H_
#define PASTEVENTS_H_

#define MAX_HISTORY 15

struct pair
{
    int histCount;
    char **history;
};

int open_history_file();
void save_command(char *);

#endif