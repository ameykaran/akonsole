#ifndef PROCLORE_H_
#define PROCLORE_H_

void proclore(char *pid);

typedef struct procInfo
{
    char exec[PATH_MAX];
    long pid;
    long ppid;
    char status[5]; /*Status of the process*/
    int fgbg;          /*Foreground or background*/
    long vmsize;       /*Virtual memory size*/
} procInfo;

#endif