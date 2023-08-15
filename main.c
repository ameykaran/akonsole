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

    strcpy(CURR_DIR, HOME_DIR);

    char buf[PATH_MAX];
    printf("%s\n", getcwd(buf, PATH_MAX));
    cd("~/new");
    printf("%s\n", getcwd(buf, PATH_MAX));
    cd("..");
    printf("%s\n", getcwd(buf, PATH_MAX));
    cd("");
    printf("%s\n", getcwd(buf, PATH_MAX));
}