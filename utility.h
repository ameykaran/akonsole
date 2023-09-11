
#ifndef UTILITY_H_
#define UTILITY_H_

char *lstrip(char *str, char letter);
char *rstrip(char *str, char letter);
char *strip(char *str, char letter);
void print_error(char *error);
char *get_abs_path(char *path);
char *get_rel_path(char *path);
char *remove_whitespaces(char *text);

/*
Prints the given num as a digits-digit number
*/
void print_aligned(long num, int digits);
char *trim(char *string, char *prefix);

void print_last_exec_output();
int readString(const char *str, char *res);

typedef struct processNode
{
    int pid;
    char pName[256];
    char isBg, isRunning;
    time_t spawnTime;
    struct processNode *next, *prev;

} processNode;

typedef struct processList
{
    struct processNode *head;
    struct processNode *tail;
    int size;
} processList;

void insert_process(int pid, int argc, char *argv, char isBg);
// processNode *get_f();

processNode *get_process_with_id(int pid);
void remove_process_with_id(int pid);

#endif