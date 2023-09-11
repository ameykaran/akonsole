#ifndef COMMAND_HANDLER_H_
#define COMMAND_HANDLER_H_

typedef int (*execHandler)(int argv, char *argc[]);

typedef enum customCommands
{
    UNKNOWN,
    WARP,
    PEEK,
    PASTEVENTS,
    PROCLORE,
    SEEK,
    IMAN,
    ACTIVITIES,
    PING,
    FG,
    BG,
    NEONATE,
    EXIT
} customCommands;

#define IO_INP 1
#define IO_INP_TEXT 2
#define IO_OUT 3
#define IO_APP 4

typedef struct cmdMap
{
    char *cmdName;
    customCommands cmdID;
    execHandler handler;
} cmdMap;

typedef struct IOQuadrio
{
    char *inp, *inpText, *app, *out;
} IOQuadrio;

void execute_multi_line_command(char *cmd);

#define RESET_IO_REDIRECTION         \
    dup2(inpBackup, STDIN_FILENO);  \
    dup2(outBackup, STDOUT_FILENO); \
    close(inpBackup);               \
    close(inpTextBackup);           \
    close(outBackup);

#endif