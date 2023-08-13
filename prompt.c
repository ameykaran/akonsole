#include "headers.h"

extern char HOME_DIR[PATH_MAX], CURR_DIR[PATH_MAX], PREV_DIR[PATH_MAX];

void prompt()
{
    char host[1024];
    gethostname(host, 1024);
    char *user = getenv("USER");
    if (user == NULL)
        user = "default";

    printf("<" ANSI_FG_COLOR_CYAN "%s" ANSI_COLOR_RESET "@%s:%s> ", user, host, CURR_DIR);
}
