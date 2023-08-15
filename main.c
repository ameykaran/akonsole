#include "headers.h"

char HOME_DIR[PATH_MAX], CURR_DIR[PATH_MAX], PREV_DIR[PATH_MAX];

int main()
{
    system("clear");

    if (!getcwd(HOME_DIR, PATH_MAX))
    {
        printf("Unable to determine the home directory!");
        exit(1);
    }

    strcpy(CURR_DIR, "~");

    // printf("%d\n", read_history()->histCount);
    save_command("pastevents\n");
    save_command("gcc main.c -o main");
    save_command("ls .");
}