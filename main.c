#include "headers.h"
#include "prompt.h"


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
    prompt();
}