#include "headers.h"

char HOME_DIR[PATH_MAX] = {}, CURR_DIR[PATH_MAX] = {}, PREV_DIR[PATH_MAX] = {};
long TERMINAL_PID;

int main()
{
    system("clear");

    if (!getcwd(HOME_DIR, PATH_MAX))
    {
        printf("Unable to determine the home directory!");
        exit(1);
    }

    strcpy(CURR_DIR, HOME_DIR);
    TERMINAL_PID = getpid();

    char buffer[ARG_MAX];
    while (1)
    {
        prompt();
        fgets(buffer, ARG_MAX, stdin);
        execute_multi_command(buffer);
    }
}