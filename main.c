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

    // printLogo();

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

    set_signal_handlers();

    char buffer[ARG_MAX];
    while (1)
    {
        prompt();
        if (!fgets(buffer, ARG_MAX, stdin))
            ctrl_d_handler(buffer);
        print_last_exec_output();
        execute_multi_line_command(buffer);
        memset(buffer, 0, ARG_MAX);
    }
}