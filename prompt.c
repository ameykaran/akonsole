#include "headers.h"

void prompt()
{

    char host[1024];
    gethostname(host, 1024);
    char username[1024];

    uid_t uid = getuid();
    struct passwd *user = getpwuid(uid);
    strcpy(username, user ? user->pw_name : "default");

    printf("<" ANSI_FG_COLOR_CYAN "%s" ANSI_COLOR_RESET "@%s:%s> ", username, host, get_rel_path(CURR_DIR));
}
