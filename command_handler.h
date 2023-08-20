#ifndef COMMAND_HANDLER_H_
#define COMMAND_HANDLER_H_

typedef int (*execHandler)(const char *args);

typedef enum customCommands
{
    UNKNOWN,
    WARP,
    PEEK,
    PASTEVENTS,
    PROCLORE,
    SEEK,
    EXIT
} customCommands;

typedef struct cmdMap
{
    char *cmdName;
    customCommands cmdID;
    execHandler handler;
} cmdMap;

void execute_multi_command(char *cmd);

#endif