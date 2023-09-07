#include "headers.h"

char HOME_DIR[PATH_MAX] = {}, CURR_DIR[PATH_MAX] = {}, PREV_DIR[PATH_MAX] = {};
long TERMINAL_PID;
char PREV_COMMAND[1024] = {0};
char PREV_COMMAND_OUTPUT[1024];
processList list;
processList *Processes = &list;

int main()
{
    printf("\033c");

    printLogo();

    if (!getcwd(HOME_DIR, PATH_MAX))
    {
        printf("Unable to determine the home directory!");
        exit(1);
    }

    strcpy(CURR_DIR, HOME_DIR);
    TERMINAL_PID = getpid();

    strcpy(PREV_COMMAND_OUTPUT, CURR_DIR);
    strcat(PREV_COMMAND_OUTPUT, "/output");

    Processes->head = NULL;
    Processes->tail = NULL;
    Processes->size = 0;

    struct sigaction bgProcessFinishSig = {0};

    bgProcessFinishSig.sa_handler = kill_children;
    sigemptyset(&bgProcessFinishSig.sa_mask);
    bgProcessFinishSig.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &bgProcessFinishSig, NULL);

    struct sigaction exitSig = {0};
    exitSig.sa_handler = kill_terminal;
    sigemptyset(&exitSig.sa_mask);
    exitSig.sa_flags = SA_RESTART;
    sigaction(SIGINT, &exitSig, NULL);

    char buffer[ARG_MAX];
    while (1)
    {
        prompt();
        fgets(buffer, ARG_MAX, stdin);
        print_last_exec_output();
        execute_multi_line_command(buffer);
        memset(buffer, 0, ARG_MAX);
    }
}