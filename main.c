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
    // printf("%d\n", TERMINAL_PID);

    char buffer[1024];
    sprintf(buffer, "%ld", TERMINAL_PID);

    // prompt();
    // cd("-");
    // cd("..");
    // cd("..");
    // ls(".", 0, 1);
    // find("hello", "..");
    // find("hello", ".");
    // proclore(buffer);
    proclore("396429");
    // while (1)
    ;
}