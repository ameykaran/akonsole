#include "headers.h"

char HOME_DIR[PATH_MAX] = {}, CURR_DIR[PATH_MAX] = {}, PREV_DIR[PATH_MAX] = {};
long TERMINAL_PID;
char PREV_COMMAND[1024] = {0};
char PREV_COMMAND_OUTPUT[1024];
processList list;
processList *bgProcesses = &list;

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

    bgProcesses->head = NULL;
    bgProcesses->tail = NULL;
    bgProcesses->size = 0;

    struct sigaction new_action = {0};

    new_action.sa_handler = kill_children;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &new_action, NULL);

    char buffer[ARG_MAX];
    while (1)
    {

        prompt();
        fgets(buffer, ARG_MAX, stdin);
        print_last_exec_output();
        execute_multi_line_command(buffer);
    }
}