#ifndef PEEK_H_
#define PEEK_H_

#define DIR_MAX_ENTRIES 2

typedef struct directEntry directEntry;

struct directEntry
{
    char perms[10];
    long numLinks;
    char user[NAME_MAX];
    char group[NAME_MAX];
    long size;
    time_t timeCreated;
    char name[PATH_MAX];
};

void ls(char *, int, int);

#endif