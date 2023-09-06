#ifndef FGBG_H_
#define FGBG_H_

typedef struct processNode
{
    int pid;
    char pName[256];
    char isBg;
    time_t spawnTime;
    struct processNode *next;
} processNode;

typedef struct processList
{
    struct processNode *head;
    struct processNode *tail;
    int size;
} processList;

void insert_process(int pid, char *cmd, char isBg);
// processNode *get_f();

processNode *get_process_with_id(int pid);
void remove_process_with_id(int pid);

#endif